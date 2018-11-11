#ifndef PNG_WRITER_H_
#define PNG_WRITER_H_

#include "png.h"
#include "duplication_manager.h"

namespace png
{
	// д��png�ļ�
	// buffer ������byte���飬˳��ΪBGRA
	bool WritePng(png_bytep buffer, int width, int height, const char* file_name);
	void SaveFrameAsPng(dupl::FrameData* frame, const unsigned hash);
}

#endif