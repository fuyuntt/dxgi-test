#ifndef PNG_WRITER_H_
#define PNG_WRITER_H_

#include "png.h"
namespace png
{
	// д��png�ļ�
	// buffer ������byte���飬˳��ΪBGRA
	bool WritePng(png_bytep buffer, int width, int height, const char* file_name);
}

#endif