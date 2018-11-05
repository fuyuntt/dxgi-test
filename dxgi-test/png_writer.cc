#include "stdafx.h"
#include "png_writer.h"
#include "logger.h"

#define BIT_DEPTH 8
#define BYTES_PER_PIXEL 4
namespace png
{
	bool WritePng(png_bytep buffer, int width, int height, const char* file_name)
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
		png_set_IHDR(png_ptr, png_info_ptr, width, height, BIT_DEPTH, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		// 写入文件头部信息
		png_write_info(png_ptr, png_info_ptr);
		png_bytep change_buffer = new png_byte[width * height * BYTES_PER_PIXEL];
		for (int i = 0; i < width * height; i++)
		{
			png_bytep src = buffer + i * BYTES_PER_PIXEL;
			png_bytep dest = change_buffer + i * BYTES_PER_PIXEL;
			dest[0] = src[2];
			dest[1] = src[1];
			dest[2] = src[0];
			dest[3] = src[3];
		}
		png_bytep* row_pointers = new png_bytep[height];
		for (int k = 0; k < height; k++)
		{
			row_pointers[k] = change_buffer + k * width * BYTES_PER_PIXEL;
		}
		// 写入像素内容
		png_write_image(png_ptr, row_pointers);
		delete[] row_pointers;
		delete[] change_buffer;
		// 写入文件尾
		png_write_end(png_ptr, png_info_ptr);
		// 释放内存
		png_destroy_write_struct(&png_ptr, &png_info_ptr);
		fclose(file);
		return true;
	}
}