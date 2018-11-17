#include "stdafx.h"
#include "game.h"

#define COUNT_SECONDS 10

namespace game {
	const int kTimeout = 500;

	POINT BeginPoint(const RECT& rect)
	{
		return POINT{ rect.left, rect.top };
	}
	bool IsInRect(const RECT& rect, const POINT& p)
	{
		return p.x <= rect.right && p.y <= rect.bottom;
	}
	void Next(const RECT& rect, POINT* p)
	{
		p->x = p->x < rect.right ? p->x + 1 : rect.left;
		p->y = p->x == rect.left ? p->y + 1 : p->y;
	}
	static log4c::Logger logger("GameController");
	Controller::Controller() : dupl_manager_(nullptr)
	{
	}

	Controller::~Controller()
	{
		if (dupl_manager_ != NULL)
		{
			delete dupl_manager_;
		}
		if (keyboard_mouse_ != NULL)
		{
			delete keyboard_mouse_;
		}
		if (png_writer_ != NULL)
		{
			delete png_writer_;
		}
	}

	ReturnStatus Controller::Init(std::string picture_dir)
	{
		logger.Info("��ʼ��ʼ����Ϸ������ģ��");
		ReturnStatus st = ERROR_EXPECTED;
		while (st == ERROR_EXPECTED)
		{
			if (dupl_manager_ != NULL)
			{
				delete dupl_manager_;
			}
			dupl_manager_ = new dupl::DuplicationManager();
			st = dupl_manager_->Init(0);
			if (st == ERROR_EXPECTED)
			{
				logger.Info("���渴�Ƴ�ʼ�������ɻָ��쳣�����³�ʼ��");
				Sleep(500);
			}
		}
		if (st == ERROR_UNEXPECTED)
		{
			logger.Error("��ʼ�����渴���������ɻָ��쳣");
			return st;
		}
		if (png_writer_ == NULL)
		{
			png_writer_ = new png::PngWriter();
			png_writer_->Init(picture_dir);
		}
		if (keyboard_mouse_ == NULL)
		{
			keyboard_mouse_ = new km::KeyboardMouse();
			if (!(keyboard_mouse_->Init()))
			{
				return ERROR_UNEXPECTED;
			}
		}
		logger.Info("��Ϸ��������ʼ���ɹ�");
		return SUCCESS;
	}

	void Controller::AddFilter(Filter* filter)
	{
		filters_.push_back(filter);
	}

	ReturnStatus Controller::StartGaming()
	{
		dupl::FrameData frame_data;
		Context context = {keyboard_mouse_, png_writer_, NONE, false, 0};
		DWORD next_tick = GetTickCount() + COUNT_SECONDS * 1000;
		DWORD count = 0;
		while (true)
		{
			count++;
			DWORD cur_tick = GetTickCount();
			if (cur_tick >= next_tick)
			{
				logger.Debug("��ǰ����֡��[%d]", count / COUNT_SECONDS);
				next_tick = cur_tick + COUNT_SECONDS * 1000;
				count = 0;
			}
			bool is_timeout;
			dupl_manager_->DoneWithFrame();
			ReturnStatus st = dupl_manager_->GetFrame(kTimeout, &frame_data, &is_timeout);
			if (st != SUCCESS)
			{
				return st;
			}
			if (is_timeout)
			{
				continue;
			}
			context.tick_count = cur_tick;
			std::vector<Filter*>::iterator it;
			for (it = filters_.begin(); it != filters_.end(); it++)
			{
				bool is_continue = (*it)->Run(is_timeout ? NULL : &frame_data, &context);
				if (!is_continue)
					break;
			}
		}
		dupl_manager_->DoneWithFrame();
	}
}