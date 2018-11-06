#include "stdafx.h"
#include "game.h"

namespace game {
	const int kTimeout = 10;
	Controller::Controller() : dupl_manager(nullptr)
	{
	}

	Controller::~Controller()
	{
		delete dupl_manager;
	}

	ReturnStatus Controller::Init()
	{
		dupl_manager = new dupl::DuplicationManager();
		logger::info("��ʼ��ʼ�����渴��");
		ReturnStatus status = dupl_manager->Init(0);
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
		filters.push_back(filter);
	}

	ReturnStatus Controller::StartGaming()
	{
		dupl::FrameData frame_data;

		while (true)
		{
			bool isTimeout;
			ReturnStatus st = dupl_manager->GetFrame(kTimeout, &frame_data, &isTimeout);
			if (st != SUCCESS)
			{
				return st;
			}
			std::vector<Filter*>::iterator it;
			Context context;
			for (it = filters.begin(); it != filters.end(); it++)
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
			std::vector<Filter*>::reverse_iterator r_it;
			for (r_it = filters.rbegin(); r_it != filters.rend(); r_it--)
			{
				(*r_it)->ClearContext(&context);
			}
		}
	}
}