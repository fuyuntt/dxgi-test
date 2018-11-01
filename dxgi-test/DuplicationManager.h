#pragma once
#include "CommonTypes.h"

//
// Handles the task of duplicating an output.
//
class DUPLICATIONMANAGER
{
public:
	DUPLICATIONMANAGER();
	~DUPLICATIONMANAGER();
	DUPL_RETURN GetFrame(_Out_ FRAME_DATA* FrameData, _Out_ bool* Timeout);
	DUPL_RETURN InitDupl(UINT Output);
	void ReleaseDupl();
	void GetOutputDesc(_Out_ DXGI_OUTPUT_DESC* DescPtr);

private:

	// vars
	IDXGIOutputDuplication* m_DeskDupl;
	DXGI_OUTDUPL_DESC m_DuplDesc;
	UINT m_OutputNumber;
	DXGI_OUTPUT_DESC m_OutputDesc;
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_Context;
};
