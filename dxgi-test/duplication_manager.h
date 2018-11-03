#ifndef __DXGI_MGR_H_
#define __DXGI_MGR_H_

#include "common.h"
#include <stdio.h>
#include <d3d11.h>
#include <dxgi1_2.h>

#define RELEASE(lpUnk) do {if ((lpUnk) != NULL) { (lpUnk)->Release(); (lpUnk) = nullptr;}} while (0)
//
// FRAME_DATA holds information about an acquired frame
//
struct FrameData
{
	const BYTE* buffer;
	int height;
	int width;
};

enum DuplReturn
{
	DUPL_RETURN_SUCCESS = 0,
	DUPL_RETURN_ERROR_EXPECTED = 1,
	DUPL_RETURN_ERROR_UNEXPECTED = 2
};

DuplReturn ProcessFailure(_In_ ID3D11Device* device, _In_ LPCWSTR str, _In_ LPCWSTR title, HRESULT hr, _In_ HRESULT* expected_errors = nullptr);

//
// Handles the task of duplicating an output.
//
class DuplicationManager
{
public:
	DuplicationManager();
	~DuplicationManager();
	DuplReturn GetFrame(_In_ int timeout, _Out_ FrameData* frame_data, _Out_ bool* is_timeout);
	DuplReturn Init(UINT output);
	void ReleaseDupl();
	void GetOutputDesc(_Out_ DXGI_OUTPUT_DESC* output_desc);

private:
	// vars
	IDXGIOutputDuplication* desk_dupl_;
	DXGI_OUTDUPL_DESC dupl_desc_;
	UINT output_number_;
	DXGI_OUTPUT_DESC output_desc_;
	ID3D11Device* d3d_device_;
	ID3D11DeviceContext* d3d_device_context_;
};
#endif