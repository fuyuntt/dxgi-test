#include "stdafx.h"
#include "duplication_manager.h"
#include "message.h"
#include "logger.h"

#define RELEASE(lpUnk) do {if ((lpUnk) != NULL) { (lpUnk)->Release(); (lpUnk) = nullptr;}} while (0)
namespace dupl 
{
	// Below are lists of errors expect from Dxgi API calls when a transition event like mode change, PnpStop, PnpStart
	// desktop switch, TDR or session disconnect/reconnect. In all these cases we want the application to clean up the threads that process
	// the desktop updates and attempt to recreate them.
	// If we get an error that is not on the appropriate list then we exit the application

	// These are the errors we expect from general Dxgi API due to a transition
	HRESULT SystemTransitionsExpectedErrors[] = {
		DXGI_ERROR_DEVICE_REMOVED,
		DXGI_ERROR_ACCESS_LOST,
		static_cast<HRESULT>(WAIT_ABANDONED),
		S_OK                                    // Terminate list with zero valued HRESULT
	};

	// These are the errors we expect from IDXGIOutput1::DuplicateOutput due to a transition
	HRESULT CreateDuplicationExpectedErrors[] = {
		DXGI_ERROR_DEVICE_REMOVED,
		static_cast<HRESULT>(E_ACCESSDENIED),
		DXGI_ERROR_UNSUPPORTED,
		DXGI_ERROR_SESSION_DISCONNECTED,
		S_OK                                    // Terminate list with zero valued HRESULT
	};

	// These are the errors we expect from IDXGIOutputDuplication methods due to a transition
	HRESULT FrameInfoExpectedErrors[] = {
		DXGI_ERROR_DEVICE_REMOVED,
		DXGI_ERROR_ACCESS_LOST,
		S_OK                                    // Terminate list with zero valued HRESULT
	};

	// These are the errors we expect from IDXGIAdapter::EnumOutputs methods due to outputs becoming stale during a transition
	HRESULT EnumOutputsExpectedErrors[] = {
		DXGI_ERROR_NOT_FOUND,
		S_OK                                    // Terminate list with zero valued HRESULT
	};

	//
	// Constructor sets up references / variables
	//
	DuplicationManager::DuplicationManager() : desk_dupl_(nullptr), output_number_(0), d3d_device_(nullptr), d3d_device_context_(nullptr), staging_surf_(nullptr)
	{
		RtlZeroMemory(&output_desc_, sizeof(output_desc_));
	}

	//
	// Destructor simply calls CleanRefs to destroy everything
	//
	DuplicationManager::~DuplicationManager()
	{
		DoneWithFrame();
		if (desk_dupl_)
		{
			RELEASE(desk_dupl_);
		}

		if (d3d_device_)
		{
			RELEASE(d3d_device_);
		}
	}

	//
	// Initialize duplication interfaces
	//
	ReturnStatus DuplicationManager::Init(UINT output)
	{
		D3D11CreateDevice(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			NULL,
			NULL,
			NULL,
			D3D11_SDK_VERSION,
			&d3d_device_,
			NULL,
			&d3d_device_context_);
		output_number_ = output;

		// Get DXGI device
		IDXGIDevice* dxgi_device = nullptr;
		HRESULT hr = d3d_device_->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgi_device));
		if (FAILED(hr))
		{
			return ProcessFailure(nullptr, L"Failed to QI for DXGI Device", hr);
		}

		// Get DXGI adapter
		IDXGIAdapter* dxgi_adapter = nullptr;
		hr = dxgi_device->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgi_adapter));
		RELEASE(dxgi_device);
		if (FAILED(hr))
		{
			return ProcessFailure(d3d_device_, L"Failed to get parent DXGI Adapter", hr, SystemTransitionsExpectedErrors);
		}

		// Get output
		IDXGIOutput* dxgi_output = nullptr;
		hr = dxgi_adapter->EnumOutputs(output, &dxgi_output);
		RELEASE(dxgi_adapter);
		if (FAILED(hr))
		{
			return ProcessFailure(d3d_device_, L"Failed to get specified output in DUPLICATIONMANAGER", hr, EnumOutputsExpectedErrors);
		}

		dxgi_output->GetDesc(&output_desc_);

		// QI for Output 1
		IDXGIOutput1* dxgi_output1 = nullptr;
		hr = dxgi_output->QueryInterface(__uuidof(dxgi_output1), reinterpret_cast<void**>(&dxgi_output1));
		RELEASE(dxgi_output);
		if (FAILED(hr))
		{
			return ProcessFailure(nullptr, L"Failed to QI for DxgiOutput1 in DUPLICATIONMANAGER", hr);
		}

		// Create desktop duplication
		hr = dxgi_output1->DuplicateOutput(d3d_device_, &desk_dupl_);
		RELEASE(dxgi_output1);
		if (FAILED(hr))
		{
			if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
			{
				msg::ShowMsg(L"There is already the maximum number of applications using the Desktop Duplication API running, please close one of those applications and then try again.");
				return ERROR_UNEXPECTED;
			}
			return ProcessFailure(d3d_device_, L"Failed to get duplicate output in DUPLICATIONMANAGER", hr, CreateDuplicationExpectedErrors);
		}
		return SUCCESS;
	}

	//
	// Get next frame and write it into Data
	//
	ReturnStatus DuplicationManager::GetFrame(_In_ int timeout, _Out_ FrameData* frame_data, _Out_ bool* is_timeout)
	{
		if (staging_surf_ != nullptr)
		{
			logger::error("请先释放之前的画面");
			return ERROR_UNEXPECTED;
		}
		IDXGIResource* desktop_resource = nullptr;
		DXGI_OUTDUPL_FRAME_INFO frame_info;

		// Get new frame
		HRESULT hr = desk_dupl_->AcquireNextFrame(timeout, &frame_info, &desktop_resource);
		if (hr == DXGI_ERROR_WAIT_TIMEOUT)
		{
			*is_timeout = true;
			return SUCCESS;
		}
		*is_timeout = false;

		if (FAILED(hr))
		{
			return ProcessFailure(d3d_device_, L"Failed to acquire next frame in DUPLICATIONMANAGER", hr, FrameInfoExpectedErrors);
		}
		//
		// query next frame staging buffer
		//
		ID3D11Texture2D *acquired_desktop_image = NULL;
		hr = desktop_resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&acquired_desktop_image));
		RELEASE(desktop_resource);

		if (FAILED(hr))
		{
			return ProcessFailure(nullptr, L"Failed to QI for ID3D11Texture2D from acquired IDXGIResource in DUPLICATIONMANAGER", hr);
		}

		//
		// copy old description
		//
		D3D11_TEXTURE2D_DESC frame_descriptor;
		acquired_desktop_image->GetDesc(&frame_descriptor);

		//
		// create a new staging buffer for fill frame image
		//
		ID3D11Texture2D *new_desktop_image = NULL;
		frame_descriptor.Usage = D3D11_USAGE_STAGING;
		frame_descriptor.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		frame_descriptor.BindFlags = 0;
		frame_descriptor.MiscFlags = 0;
		frame_descriptor.MipLevels = 1;
		frame_descriptor.ArraySize = 1;
		frame_descriptor.SampleDesc.Count = 1;
		hr = d3d_device_->CreateTexture2D(&frame_descriptor, NULL, &new_desktop_image);
		if (FAILED(hr))
		{
			return ProcessFailure(nullptr, L"创建cpu可访问材质异常", hr);
		}

		//
		// copy next staging buffer to new staging buffer
		//
		d3d_device_context_->CopyResource(new_desktop_image, acquired_desktop_image);
		RELEASE(acquired_desktop_image);
		desk_dupl_->ReleaseFrame();
		//
		// create staging buffer for map bits
		//
		new_desktop_image->QueryInterface(__uuidof(IDXGISurface), (void **)(&staging_surf_));
		RELEASE(new_desktop_image);
		if (FAILED(hr))
		{
			return ProcessFailure(nullptr, L"创建staging buffer失败", hr);
		}
		//
		// copy bits to user space
		//
		DXGI_MAPPED_RECT mapped_rect;
		hr = staging_surf_->Map(&mapped_rect, DXGI_MAP_READ);
		if (FAILED(hr))
		{
			RELEASE(staging_surf_);
			return ProcessFailure(nullptr, L"map staging_surf 失败", hr);
		}
		frame_data->buffer = mapped_rect.pBits;
		frame_data->width = frame_descriptor.Width;
		frame_data->height = frame_descriptor.Height;
		return SUCCESS;
	}

	void DuplicationManager::DoneWithFrame()
	{
		if (staging_surf_ != NULL)
		{
			staging_surf_->Unmap();
			RELEASE(staging_surf_);
		}
	}

	//
	// Gets output desc into DescPtr
	//
	void DuplicationManager::GetOutputDesc(_Out_ DXGI_OUTPUT_DESC* output_desc)
	{
		*output_desc = output_desc_;
	}

	ReturnStatus ProcessFailure(_In_opt_ ID3D11Device* device, _In_ LPCWSTR str, HRESULT hr, _In_ HRESULT* expected_errors)
	{
		HRESULT translated_hr;

		// On an error check if the DX device is lost
		if (device)
		{
			HRESULT device_removed_reason = device->GetDeviceRemovedReason();

			switch (device_removed_reason)
			{
			case DXGI_ERROR_DEVICE_REMOVED:
			case DXGI_ERROR_DEVICE_RESET:
			case static_cast<HRESULT>(E_OUTOFMEMORY) :
			{
				// Our device has been stopped due to an external event on the GPU so map them all to
				// device removed and continue processing the condition
				translated_hr = DXGI_ERROR_DEVICE_REMOVED;
				break;
			}

			case S_OK:
			{
				// Device is not removed so use original error
				translated_hr = hr;
				break;
			}

			default:
			{
				// Device is removed but not a error we want to remap
				translated_hr = device_removed_reason;
			}
			}
		}
		else
		{
			translated_hr = hr;
		}

		// Check if this error was expected or not
		if (expected_errors)
		{
			HRESULT* current_result = expected_errors;

			while (*current_result != S_OK)
			{
				if (*(current_result++) == translated_hr)
				{
					return ERROR_EXPECTED;
				}
			}
		}

		// Error was not expected so display the message box
		msg::ShowFailure(str, translated_hr);

		return ERROR_UNEXPECTED;
	}
}
