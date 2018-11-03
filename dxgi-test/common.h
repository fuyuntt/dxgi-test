#ifndef __DXGI_COMMON_H_
#define __DXGI_COMMON_H_

#include <windows.h>
#include "png.h"
// ��ʾ��Ϣ
void DisplayMsg(const wchar_t* message, const wchar_t* title, HRESULT hr);

// дPng
int WritePng(png_bytep buffer, int width, int height, const char* file_name);
#endif