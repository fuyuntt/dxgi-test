#ifndef __DXGI_COMMON_H_
#define __DXGI_COMMON_H_

#include <windows.h>
#include <stdio.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <sal.h>

#define RELEASE(lpUnk) do {if ((lpUnk) != NULL) { (lpUnk)->Release(); (lpUnk) = nullptr;}} while (0)

extern HRESULT SystemTransitionsExpectedErrors[];
extern HRESULT CreateDuplicationExpectedErrors[];
extern HRESULT FrameInfoExpectedErrors[];
extern HRESULT AcquireFrameExpectedError[];
extern HRESULT EnumOutputsExpectedErrors[];

typedef _Return_type_success_(return == DUPL_RETURN_SUCCESS) enum
{
	DUPL_RETURN_SUCCESS = 0,
	DUPL_RETURN_ERROR_EXPECTED = 1,
	DUPL_RETURN_ERROR_UNEXPECTED = 2
}DUPL_RETURN;

//
// FRAME_DATA holds information about an acquired frame
//
typedef struct _FRAME_DATA
{
	PBYTE pBuf;
	int Height;
	int Width;
} FRAME_DATA;

_Post_satisfies_(return != DUPL_RETURN_SUCCESS)
DUPL_RETURN ProcessFailure(_In_opt_ ID3D11Device* Device, _In_ LPCWSTR Str, _In_ LPCWSTR Title, HRESULT hr, _In_opt_z_ HRESULT* ExpectedErrors = nullptr);

void DisplayMsg(_In_ LPCWSTR Str, _In_ LPCWSTR Title, HRESULT hr);
#endif