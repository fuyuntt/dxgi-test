#include "stdafx.h"
#include "common.h"

//
// Displays a message
//
void DisplayMsg(const wchar_t* message, const wchar_t* titile, HRESULT hr)
{
	if (SUCCEEDED(hr))
	{
		MessageBoxW(nullptr, message, titile, MB_OK);
		return;
	}

	const UINT string_len = (UINT)(wcslen(message) + sizeof(" with HRESULT 0x########."));
	wchar_t* out_string = new wchar_t[string_len];
	if (!out_string)
	{
		return;
	}

	INT len_written = swprintf_s(out_string, string_len, L"%s with 0x%X.", message, hr);
	if (len_written != -1)
	{
		MessageBoxW(nullptr, out_string, titile, MB_OK);
	}

	delete[] out_string;
}
