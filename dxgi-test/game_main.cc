// dxgi-test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "duplication_manager.h"
#include "png_writer.h"
#include "time.h"
#include "logger.h"
#include "message.h"
#include "game.h"
#include "kml.h"
#include "filters.h"

int main()
{
	logger::info("��ʼ����");

	ReturnStatus status = SUCCESS;
	game::Controller controller;
	while(status != ERROR_UNEXPECTED)
	{
		logger::info("��ʼ��ʼ��");
		status = controller.Init();
		if (status == ERROR_UNEXPECTED)
		{
			logger::error("��ʼ��ʧ��");
			msg::ShowMsg(L"���渴�Ƴ�ʼ��ʧ��");
			continue;
		}
		logger::info("��ʼ���ɹ�");
		controller.AddFilter(new game::WeaponFilter());
		controller.AddFilter(new game::IsInRangeFilter());
		status = controller.StartGaming();
		if (status == ERROR_EXPECTED)
		{
			logger::info("ϵͳ����˲̬����ʼ���³�ʼ��");
		}
	}
	system("pause");
    return 0;
}
int main_bak()
{
	logger::info("��ʼ����");
	if (!kml::LoadDLL())
	{
		goto end;
	}
	if (!kml::InitDevice())
	{
		goto end;
	}
	while (true)
	{
		kml::KeyPress(" ", 1);
		Sleep(1000);
	}
end:
	system("pause");
	return 0;
}
