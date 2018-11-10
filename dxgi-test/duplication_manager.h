#ifndef __DXGI_MGR_H_
#define __DXGI_MGR_H_

#include <d3d11.h>
#include <dxgi1_2.h>
#include "common.h"

namespace dupl 
{
	struct COLOR_BGRA
	{
		BYTE b;
		BYTE g;
		BYTE r;
		BYTE a;
		bool operator==(const COLOR_BGRA& bgra) const {
			return b == bgra.b &&
				g == bgra.g &&
				r == bgra.r &&
				a == bgra.a;
		}
	};
	//
	// FRAME_DATA holds information about an acquired frame
	// Ã¿¸öÏñËØ4 byte
	//
	class FrameData
	{
	public:
		BYTE* buffer;
		int height;
		int width;

		COLOR_BGRA* GetPixel(const POINT& p)
		{
			return reinterpret_cast<COLOR_BGRA*>(buffer) + p.y * width + p.x;
		}


	};


	ReturnStatus ProcessFailure(_In_ ID3D11Device* device, _In_ LPCWSTR str, HRESULT hr, _In_ HRESULT* expected_errors = nullptr);

	//
	// Handles the task of duplicating an output.
	//
	class DuplicationManager
	{
	public:
		DuplicationManager();
		~DuplicationManager();
		ReturnStatus GetFrame(_In_ int timeout, _Out_ FrameData* frame_data, _Out_ bool* is_timeout);
		void DoneWithFrame();
		ReturnStatus Init(UINT output);
		void GetOutputDesc(_Out_ DXGI_OUTPUT_DESC* output_desc);

	private:
		// vars
		IDXGIOutputDuplication* desk_dupl_;
		DXGI_OUTDUPL_DESC dupl_desc_;
		UINT output_number_;
		DXGI_OUTPUT_DESC output_desc_;
		IDXGISurface *staging_surf_;

		ID3D11Device* d3d_device_;
		ID3D11DeviceContext* d3d_device_context_;
	};
}
#endif
