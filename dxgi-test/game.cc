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
	// дͼƬ�߳�
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
		logger::info("weapon [%u] д���ļ����", hash);
		return 0;
	}

	void SaveFrameAsPng(dupl::FrameData* frame, const unsigned hash)
	{
		logger::info("��ʼ׼����weapon [%u] д���ļ�", hash);
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
		logger::info("��ʼ��ʼ�����渴��");
		ReturnStatus status = dupl_manager_->Init(0);
		if (status == ERROR_EXPECTED)
		{
			logger::info("��ʼ�����渴�������ɻָ��쳣");
			return status;
		}
		else if (status == ERROR_UNEXPECTED)
		{
			logger::error("��ʼ�����渴���������ɻָ��쳣");
			return status;
		}
		logger::info("���渴�Ƴ�ʼ���ɹ�");
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