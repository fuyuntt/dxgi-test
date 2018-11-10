// dxgi-test.cpp : 定义控制台应用程序的入口点。
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
	logger::info("开始程序");
	if (!(kml::LoadDLL() && kml::InitDevice()))
	{
		system("pause");
		return 1;
	}
	ReturnStatus status = SUCCESS;
	game::Controller controller;
	while(status != ERROR_UNEXPECTED)
	{
		logger::info("开始初始化");
		status = controller.Init();
		if (status == ERROR_UNEXPECTED)
		{
			logger::error("初始化失败");
			msg::ShowMsg(L"桌面复制初始化失败");
			continue;
		}
		logger::info("初始化成功");
		controller.AddFilter(new game::WeaponFilter());
		controller.AddFilter(new game::IsInRangeFilter());
		controller.AddFilter(new game::SniperFilter());
		status = controller.StartGaming();
		if (status == ERROR_EXPECTED)
		{
			logger::info("系统处于瞬态，开始重新初始化");
		}
	}
	system("pause");
    return 0;
}
