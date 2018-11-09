#include "stdafx.h"
#include "game.h"
#include "process.h"
#include "png_writer.h"

namespace game {
	const int kTimeout = 10;

	bool HasNext(const RECT& rect, const POINT& p)
	{
		return p.x < rect.right || p.y < rect.bottom;
	}
	void Next(const RECT& rect, POINT* p)
	{
		p->x = p->x < rect.right ? p->x + 1 : rect.left;
		p->y = p->x == rect.left ? p->y + 1 : p->y;
	}
	struct SavePngArg {
		unsigned hash;
		dupl::FrameData* cp_frame;
	};
	// 写图片线程
	unsigned __stdcall SavePng(void* args)
	{
		SavePngArg* arg = reinterpret_cast<SavePngArg*>(args);
		char out_file[15 + 4];
		sprintf(out_file, "%u.png", arg->hash);
		png::WritePng(arg->cp_frame->buffer, arg->cp_frame->width, arg->cp_frame->height, out_file);
		unsigned hash = arg->hash;
		delete[] arg->cp_frame->buffer;
		delete arg->cp_frame;
		delete arg;
		logger::info("weapon [%u] 写入文件完成", hash);
		return 0;
	}

	void SaveFrameAsPng(dupl::FrameData* frame, const unsigned hash)
	{
		logger::info("开始准备将weapon [%u] 写入文件", hash);
		int size = frame->height * frame->width * 4;
		BYTE* buffer = new BYTE[size];
		memcpy(buffer, frame->buffer, size);
		dupl::FrameData* cp_frame = new dupl::FrameData{ buffer, frame->height, frame->width };
		_beginthreadex(NULL, 0, SavePng, new SavePngArg{ hash, cp_frame }, 0, NULL);
	}


	Controller::Controller() : dupl_manager_(nullptr)
	{
	}

	Controller::~Controller()
	{
		if (dupl_manager_ != NULL)
		{
			delete dupl_manager_;
		}
	}

	ReturnStatus Controller::Init()
	{
		if (dupl_manager_ != NULL)
		{
			delete dupl_manager_;
		}
		dupl_manager_ = new dupl::DuplicationManager();
		logger::info("开始初始化桌面复制");
		ReturnStatus status = dupl_manager_->Init(0);
		if (status == ERROR_EXPECTED)
		{
			logger::info("初始化桌面复制遇到可恢复异常");
			return status;
		}
		else if (status == ERROR_UNEXPECTED)
		{
			logger::error("初始化桌面复制遇到不可恢复异常");
			return status;
		}
		logger::info("桌面复制初始化成功");
		return SUCCESS;
	}

	void Controller::AddFilter(Filter* filter)
	{
		filters_.push_back(filter);
	}

	ReturnStatus Controller::StartGaming()
	{
		dupl::FrameData frame_data;

		while (true)
		{
			bool isTimeout;
			ReturnStatus st = dupl_manager_->GetFrame(kTimeout, &frame_data, &isTimeout);
			if (st != SUCCESS)
			{
				return st;
			}
			std::vector<Filter*>::iterator it;
			Context context;
			for (it = filters_.begin(); it != filters_.end(); it++)
			{
				if (isTimeout)
				{
					(*it)->Run(NULL, &context);
				}
				else
				{
					(*it)->Run(&frame_data, &context);
				}
			}
			dupl_manager_->DoneWithFrame();
		}
	}
}