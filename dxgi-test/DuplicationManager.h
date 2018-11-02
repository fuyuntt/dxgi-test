#ifndef __DXGI_MGR_H_
#define __DXGI_MGR_H_

#include "Common.h"

//
// Handles the task of duplicating an output.
//
class DuplicationManager
{
public:
	DuplicationManager();
	~DuplicationManager();
	DUPL_RETURN GetFrame(_Out_ FRAME_DATA* frame_data, _Out_ bool* is_timeout);
	DUPL_RETURN InitDupl(UINT output);
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
