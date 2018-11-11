#include "stdafx.h"
#include "png_writer.h"
#include "logger.h"
#include "duplication_manager.h"
#include "process.h"

#define BIT_DEPTH 8
#define BYTES_PER_PIXEL 3

extern char* g_file_store_dir;
namespace png
{
	struct SavePngArg {
		unsigned hash;
		dupl::FrameData* cp_frame;
	};
	// 写图片线程
	unsigned __stdcall SavePng(void* args)
	{
		SavePngArg* arg = reinterpret_cast<SavePngArg*>(args);
		unsigned path_len = strlen(g_file_store_dir);
		char* out_file = new char[path_len + 15 + 4];
		sprintf(out_file, "%s%u.png", g_file_store_dir, arg->hash);
		png::WritePng(arg->cp_frame->buffer, arg->cp_frame->width, arg->cp_frame->height, out_file);
		unsigned hash = arg->hash;
		delete[] out_file;
		delete[] arg->cp_frame->buffer;
		delete arg->cp_frame;
		delete arg;
		logger::info("weapon [%u] 写入文件完成", hash);
		return 0;
	}

	void SaveFrameAsPng(dupl::FrameData* frame, const unsigned hash)
	{
		logger::info("开始准备将weapon [%u] 写入文件", hash);
		int size = frame->height * frame->width * 3;
		BYTE* buffer = new BYTE[size];
		BYTE* p_d_end = buffer + size;
		for (BYTE *p_s = frame->buffer, *p_d = buffer; p_d < p_d_end; p_s += 4, p_d += 3)
		{
			p_d[0] = p_s[2];
			p_d[1] = p_s[1];
			p_d[2] = p_s[0];
		}
		dupl::FrameData* cp_frame = new dupl::FrameData{ buffer, frame->height, frame->width };
		_beginthreadex(NULL, 0, SavePng, new SavePngArg{ hash, cp_frame }, 0, NULL);
	}

	bool WritePng(png_byte* buffer, int width, int height, const char* file_name)
	{
		FILE* file;
		/*打开需要写入的文件*/
		fopen_s(&file, file_name, "wb");
		if (file == NULL)
		{
			logger::error("文件不存在：%s", file_name);
			return false;
		}

		/* 创建并初始化 png_struct 及其所需的错误处理函数，如果你想使用默认的 stderr 和 longjump() 方法，
		* 你可以将最后三个参数设为NULL，在使用动态链接库的情况下，我们也会检测函数库版本是否在编译时使用的版本是否兼容
		*/
		png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (png_ptr == NULL)
		{
			fclose(file);
			return false;
		}
		// 分配内存并初始化图像信息数据
		png_infop png_info_ptr = png_create_info_struct(png_ptr);
		if (png_info_ptr == NULL)
		{
			fclose(file);
			png_destroy_write_struct(&png_ptr, NULL);
			return false;
		}
		// 设置错误处理。如果你在调用 png_create_write_struct() 时没有设置错误处理函数，那么这段代码是必须的
		if (setjmp(png_jmpbuf(png_ptr)))
		{
			// 如果程序跑到这里了，那么写入文件时出现了问题
			fclose(file);
			png_destroy_write_struct(&png_ptr, &png_info_ptr);
			return false;
		}
		// 设置输出控制
		png_init_io(png_ptr, file);
		// 设置IHDR
		png_set_IHDR(png_ptr, png_info_ptr, width, height, BIT_DEPTH, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		// 写入文件头部信息
		png_write_info(png_ptr, png_info_ptr);
		png_byte** row_pointers = new png_byte*[height];
		for (int k = 0; k < height; k++)
		{
			row_pointers[k] = buffer + k * width * BYTES_PER_PIXEL;
		}
		// 写入像素内容
		png_write_image(png_ptr, row_pointers);
		delete[] row_pointers;
		// 写入文件尾
		png_write_end(png_ptr, png_info_ptr);
		// 释放内存
		png_destroy_write_struct(&png_ptr, &png_info_ptr);
		fclose(file);
		return true;
	}
}