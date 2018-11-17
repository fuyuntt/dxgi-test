// dxgi-test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "duplication_manager.h"
#include "png_writer.h"
#include "time.h"
#include "logger.h"
#include "message.h"
#include "game.h"
#include "keyboard_mouse.h"
#include "filters.h"
#include "memory"

static log4c::Logger logger("main");
int main(int argc, char* argv[])
{
	char* file_store_dir = "";
	if (argc == 2)
	{
		file_store_dir = argv[1];
	}
	SetConsoleCtrlHandler(NULL, true);
	logger.Info("��ʼ����");
	ReturnStatus status = SUCCESS;
	game::Controller controller;
	controller.AddFilter(new game::WeaponFilter());
	controller.AddFilter(new game::IsInRangeFilter());
	controller.AddFilter(new game::SniperFilter());
	while(status != ERROR_UNEXPECTED)
	{
		status = controller.Init(std::string(file_store_dir));
		if (status == ERROR_UNEXPECTED)
		{
			break;
		}
		status = controller.StartGaming();
		if (status == ERROR_EXPECTED)
		{
			logger.Info("ϵͳ����˲̬����ʼ���³�ʼ��");
		}
	}
	system("pause");
    return 0;
}
