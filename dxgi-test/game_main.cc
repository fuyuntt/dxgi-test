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
	SetConsoleCtrlHandler(NULL, true);
	logger::info("��ʼ����");
	if (!(kml::LoadDLL() && kml::InitDevice()))
	{
		system("pause");
		return 1;
	}
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
		controller.AddFilter(new game::SniperFilter());
		status = controller.StartGaming();
		if (status == ERROR_EXPECTED)
		{
			logger::info("ϵͳ����˲̬����ʼ���³�ʼ��");
		}
	}
	system("pause");
    return 0;
}
