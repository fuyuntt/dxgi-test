#ifndef PNG_WRITER_H_
#define PNG_WRITER_H_

#include "png.h"
namespace png
{
	// 写入png文件
	// buffer 是像素byte数组，顺序为BGRA
	bool WritePng(png_bytep buffer, int width, int height, const char* file_name);
}

#endif