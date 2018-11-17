#ifndef PNG_WRITER_H_
#define PNG_WRITER_H_

#include "png.h"
#include "blocking_queue.h"
#include <string>

namespace png
{
	class PngWriter
	{
	public:
		void Init(std::string dir);
		void SaveBuffer(png_bytep buffer, int width, int height, std::string file_name);
	private:
		struct SavePngArg {
			std::shared_ptr<png_byte> buffer;
			int width;
			int height;
			std::string file_name;
		};
		BlockingQueue<std::shared_ptr<SavePngArg>> queue_;
		std::string dir_;
		void DealSave();
	};
}

#endif