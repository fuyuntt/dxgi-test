#include "stdafx.h"
#include "message.h"

namespace msg
{
	const wchar_t* kTitle = L"Message";

	void ShowMsg(const wchar_t* message)
	{
		ShowFailure(message, S_OK);
	}

	void ShowFailure(const wchar_t* message, HRESULT hr)
	{
		if (SUCCEEDED(hr))
		{
			MessageBoxW(nullptr, message, kTitle, MB_OK);
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
			MessageBoxW(nullptr, out_string, kTitle, MB_OK);
		}
		delete[] out_string;
	}
}