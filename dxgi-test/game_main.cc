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
#include "kml_manager.h"
#include "filters.h"

char* g_file_store_dir = "";
int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		g_file_store_dir = argv[0];
	}
	SetConsoleCtrlHandler(NULL, true);
	logger::info("��ʼ����");
	if (!(kml::LoadDLL() && kml::InitDevice()))
	{
		system("pause");
		return 1;
	}
	if (!kml::StartThread())
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
		if (status == ERROR_EXPECTED)
		{
			Sleep(1000);
			continue;
		}
		if (status == ERROR_UNEXPECTED)
		{
			logger::error("���渴�Ƴ�ʼ��ʧ��");
			break;
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
