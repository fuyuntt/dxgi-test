#ifndef SHOW_MESSAGE_H_
#define SHOW_MESSAGE_H_

#include "windows.h"

namespace msg
{
	void ShowMsg(const wchar_t* message);
	void ShowFailure(const wchar_t* message, HRESULT hr);
}

#endif
