#include "stdafx.h"
#include "DuplicationManager.h"

//
// Constructor sets up references / variables
//
DuplicationManager::DuplicationManager() : desk_dupl_(nullptr),
output_number_(0),
d3d_device_(nullptr)
{
	RtlZeroMemory(&output_desc_, sizeof(output_desc_));
}

//
// Destructor simply calls CleanRefs to destroy everything
//
DuplicationManager::~DuplicationManager()
{
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
DUPL_RETURN DuplicationManager::InitDupl(UINT Output)
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
	output_number_ = Output;
	d3d_device_->AddRef();

	// Get DXGI device
	IDXGIDevice* DxgiDevice = nullptr;
	HRESULT hr = d3d_device_->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DxgiDevice));
	if (FAILED(hr))
	{
		return ProcessFailure(nullptr, L"Failed to QI for DXGI Device", L"Error", hr);
	}

	// Get DXGI adapter
	IDXGIAdapter* DxgiAdapter = nullptr;
	hr = DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DxgiAdapter));
	RELEASE(DxgiDevice);
	if (FAILED(hr))
	{
		return ProcessFailure(d3d_device_, L"Failed to get parent DXGI Adapter", L"Error", hr, SystemTransitionsExpectedErrors);
	}

	// Get output
	IDXGIOutput* DxgiOutput = nullptr;
	hr = DxgiAdapter->EnumOutputs(Output, &DxgiOutput);
	RELEASE(DxgiAdapter);
	if (FAILED(hr))
	{
		return ProcessFailure(d3d_device_, L"Failed to get specified output in DUPLICATIONMANAGER", L"Error", hr, EnumOutputsExpectedErrors);
	}

	DxgiOutput->GetDesc(&output_desc_);

	// QI for Output 1
	IDXGIOutput1* DxgiOutput1 = nullptr;
	hr = DxgiOutput->QueryInterface(__uuidof(DxgiOutput1), reinterpret_cast<void**>(&DxgiOutput1));
	RELEASE(DxgiOutput);
	if (FAILED(hr))
	{
		return ProcessFailure(nullptr, L"Failed to QI for DxgiOutput1 in DUPLICATIONMANAGER", L"Error", hr);
	}

	// Create desktop duplication
	hr = DxgiOutput1->DuplicateOutput(d3d_device_, &desk_dupl_);
	RELEASE(DxgiOutput1);
	if (FAILED(hr))
	{
		if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
		{
			MessageBoxW(nullptr, L"There is already the maximum number of applications using the Desktop Duplication API running, please close one of those applications and then try again.", L"Error", MB_OK);
			return DUPL_RETURN_ERROR_UNEXPECTED;
		}
		return ProcessFailure(d3d_device_, L"Failed to get duplicate output in DUPLICATIONMANAGER", L"Error", hr, CreateDuplicationExpectedErrors);
	}
	return DUPL_RETURN_SUCCESS;
}

//
// Get next frame and write it into Data
//
_Success_(*Timeout == false && return == DUPL_RETURN_SUCCESS)
DUPL_RETURN DuplicationManager::GetFrame(_Out_ FRAME_DATA* FrameData, _Out_ bool* Timeout)
{
	IDXGIResource* DesktopResource = nullptr;
	DXGI_OUTDUPL_FRAME_INFO FrameInfo;

	// Get new frame
	HRESULT hr = desk_dupl_->AcquireNextFrame(500, &FrameInfo, &DesktopResource);
	if (hr == DXGI_ERROR_WAIT_TIMEOUT)
	{
		*Timeout = true;
		return DUPL_RETURN_SUCCESS;
	}
	*Timeout = false;

	if (FAILED(hr))
	{
		return ProcessFailure(d3d_device_, L"Failed to acquire next frame in DUPLICATIONMANAGER", L"Error", hr, FrameInfoExpectedErrors);
	}
	//
	// query next frame staging buffer
	//
	ID3D11Texture2D *hAcquiredDesktopImage = NULL;
	hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&hAcquiredDesktopImage));
	RELEASE(DesktopResource);

	if (FAILED(hr))
	{
		return ProcessFailure(nullptr, L"Failed to QI for ID3D11Texture2D from acquired IDXGIResource in DUPLICATIONMANAGER", L"Error", hr);
	}

	//
	// copy old description
	//
	D3D11_TEXTURE2D_DESC frameDescriptor;
	hAcquiredDesktopImage->GetDesc(&frameDescriptor);

	//
	// create a new staging buffer for fill frame image
	//
	ID3D11Texture2D *hNewDesktopImage = NULL;
	frameDescriptor.Usage = D3D11_USAGE_STAGING;
	frameDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	frameDescriptor.BindFlags = 0;
	frameDescriptor.MiscFlags = 0;
	frameDescriptor.MipLevels = 1;
	frameDescriptor.ArraySize = 1;
	frameDescriptor.SampleDesc.Count = 1;
	hr = d3d_device_->CreateTexture2D(&frameDescriptor, NULL, &hNewDesktopImage);
	if (FAILED(hr))
	{
		return ProcessFailure(nullptr, L"创建cpu可访问材质异常", L"Error", hr);
	}

	//
	// copy next staging buffer to new staging buffer
	//
	d3d_device_context_->CopyResource(hNewDesktopImage, hAcquiredDesktopImage);
	RELEASE(hAcquiredDesktopImage);
	desk_dupl_->ReleaseFrame();
	//
	// create staging buffer for map bits
	//
	IDXGISurface *hStagingSurf = NULL;
	hNewDesktopImage->QueryInterface(__uuidof(IDXGISurface), (void **)(&hStagingSurf));
	RELEASE(hNewDesktopImage);
	if (FAILED(hr))
	{
		return ProcessFailure(nullptr, L"创建staging buffer失败", L"Error", hr);
	}
	//
	// copy bits to user space
	//
	DXGI_MAPPED_RECT mappedRect;
	hr = hStagingSurf->Map(&mappedRect, DXGI_MAP_READ);
	if (SUCCEEDED(hr))
	{
		FrameData->pBuf = mappedRect.pBits;
		FrameData->Width = frameDescriptor.Width;
		FrameData->Height = frameDescriptor.Height;
		hStagingSurf->Unmap();
	}
	RELEASE(hStagingSurf);
	return DUPL_RETURN_SUCCESS;
}

void DuplicationManager::ReleaseDupl()
{
	// close and release all existing COM objects
	d3d_device_->Release();
	d3d_device_context_->Release();
}

//
// Gets output desc into DescPtr
//
void DuplicationManager::GetOutputDesc(_Out_ DXGI_OUTPUT_DESC* DescPtr)
{
	*DescPtr = output_desc_;
}


