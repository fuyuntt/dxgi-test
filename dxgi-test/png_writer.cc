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
	// дͼƬ�߳�
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
		logger::info("weapon [%u] д���ļ����", hash);
		return 0;
	}

	void SaveFrameAsPng(dupl::FrameData* frame, const unsigned hash)
	{
		logger::info("��ʼ׼����weapon [%u] д���ļ�", hash);
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
		/*����Ҫд����ļ�*/
		fopen_s(&file, file_name, "wb");
		if (file == NULL)
		{
			logger::error("�ļ������ڣ�%s", file_name);
			return false;
		}

		/* ��������ʼ�� png_struct ��������Ĵ����������������ʹ��Ĭ�ϵ� stderr �� longjump() ������
		* ����Խ��������������ΪNULL����ʹ�ö�̬���ӿ������£�����Ҳ���⺯����汾�Ƿ��ڱ���ʱʹ�õİ汾�Ƿ����
		*/
		png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (png_ptr == NULL)
		{
			fclose(file);
			return false;
		}
		// �����ڴ沢��ʼ��ͼ����Ϣ����
		png_infop png_info_ptr = png_create_info_struct(png_ptr);
		if (png_info_ptr == NULL)
		{
			fclose(file);
			png_destroy_write_struct(&png_ptr, NULL);
			return false;
		}
		// ���ô�����������ڵ��� png_create_write_struct() ʱû�����ô�����������ô��δ����Ǳ����
		if (setjmp(png_jmpbuf(png_ptr)))
		{
			// ��������ܵ������ˣ���ôд���ļ�ʱ����������
			fclose(file);
			png_destroy_write_struct(&png_ptr, &png_info_ptr);
			return false;
		}
		// �����������
		png_init_io(png_ptr, file);
		// ����IHDR
		png_set_IHDR(png_ptr, png_info_ptr, width, height, BIT_DEPTH, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		// д���ļ�ͷ����Ϣ
		png_write_info(png_ptr, png_info_ptr);
		png_byte** row_pointers = new png_byte*[height];
		for (int k = 0; k < height; k++)
		{
			row_pointers[k] = buffer + k * width * BYTES_PER_PIXEL;
		}
		// д����������
		png_write_image(png_ptr, row_pointers);
		delete[] row_pointers;
		// д���ļ�β
		png_write_end(png_ptr, png_info_ptr);
		// �ͷ��ڴ�
		png_destroy_write_struct(&png_ptr, &png_info_ptr);
		fclose(file);
		return true;
	}
}