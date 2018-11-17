#include "stdafx.h"
#include "png_writer.h"
#include "logger.h"
#include <memory>
#include <thread>

#define BIT_DEPTH 8
#define BYTES_PER_PIXEL 3

namespace png
{
	static log4c::Logger logger("PngWriter");

	// д��png�ļ�
	// buffer ������byte���飬˳��ΪBGRA
	bool WritePng(png_byte* buffer, int width, int height, const char* file_name);
	void PngWriter::Init(std::string dir)
	{
		logger.Info("��ʼ��дͼƬ�߳�");
		dir_ = dir;
		std::thread thread(&PngWriter::DealSave, this);
		thread.detach();
		logger.Info("дͼƬ�̳߳�ʼ�����");
	}

	// дͼƬ�߳�
	void PngWriter::DealSave()
	{
		while (true)
		{
			std::shared_ptr<SavePngArg> arg = queue_.take();
			std::string file_name = dir_ + arg->file_name;
			WritePng((arg->buffer).get(), arg->width, arg->height, file_name.c_str());
			logger.Info("ͼƬ [%s] д���ļ����", file_name.c_str());
		}
	}
	void PngWriter::SaveBuffer(png_bytep buffer, int width, int height, std::string file_name)
	{
		logger.Info("��ʼ׼����ͼƬ [%s] д���ļ�", file_name.c_str());
		int size = height * width * 3;
		std::shared_ptr<png_byte> buffer_dest(new png_byte[size], [](png_byte* b) {delete[] b; });
		png_byte* p_d_start = buffer_dest.get();
		png_byte* p_d_end = p_d_start + size;
		for (png_byte *p_s = buffer, *p_d = p_d_start; p_d < p_d_end; p_s += 4, p_d += 3)
		{
			p_d[0] = p_s[2];
			p_d[1] = p_s[1];
			p_d[2] = p_s[0];
		}
		queue_.put(std::shared_ptr<SavePngArg>(new SavePngArg{ buffer_dest, width, height, file_name }));
	}

	bool WritePng(png_byte* buffer, int width, int height, const char* file_name)
	{
		FILE* file;
		/*����Ҫд����ļ�*/
		fopen_s(&file, file_name, "wb");
		if (file == NULL)
		{
			logger.Error("�ļ������ڣ�%s", file_name);
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