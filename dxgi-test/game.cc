#include "stdafx.h"
#include "game.h"

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
		Context context = {NONE, false, 0};
		while (true)
		{
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
			context.tick_count = GetTickCount();
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