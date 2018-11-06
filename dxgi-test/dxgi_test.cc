// dxgi-test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "duplication_manager.h"
#include "png_writer.h"
#include "time.h"
#include "logger.h"
#include "message.h"

#define OUTPUT 0

int main()
{
	logger::info("开始程序");
	dupl::DuplicationManager* duplication_manager = new dupl::DuplicationManager();
	ReturnStatus hr;
	hr = duplication_manager->Init(OUTPUT);
	if (hr != SUCCESS)
	{
		logger::error("初始化失败");
		msg::ShowMsg(L"桌面复制程序初始化失败");
		return 1;
	}
	bool is_timeout;
	dupl::FrameData frame_data;
	time_t start_sec = time(NULL);
	time_t end_sec = start_sec + 360;
	int count = 0;
	while (time(NULL) < end_sec)
	{
		hr = duplication_manager->GetFrame(10, &frame_data, &is_timeout);
		if (hr == ERROR_EXPECTED)
		{
			logger::info("系统处于瞬态，开始重新初始化");
			duplication_manager->DoneWithFrame();
			delete duplication_manager;
			duplication_manager = new dupl::DuplicationManager();
			hr = duplication_manager->Init(OUTPUT);
			if (hr != SUCCESS)
			{
				delete duplication_manager;
				logger::error("重新初始化失败");
				msg::ShowMsg(L"重新初始化失败");
				return 1;
			}
			logger::info("初始化成功");
			continue;
		}
		else if (hr == ERROR_UNEXPECTED)
		{
			logger::error("获取数据意外失败");
			msg::ShowMsg(L"获取数据失败-意外失败");
			return 2;
		}
		count++;
		if (!is_timeout)
		{
			char path[30];
			sprintf_s(path, "d:\\temp\\pngt\\test-%d.png", count);
			bool res = png::WritePng(frame_data.buffer, frame_data.width, frame_data.height, path);
			if (!res)
			{
				logger::error("创建图片失败");
			}
			Sleep(2000);
		}
		duplication_manager->DoneWithFrame();
	}
	logger::info("创建截图%d个, 大约%d个每秒", count, count / 20);
	msg::ShowMsg(L"程序结束");
	system("pause");
    return 0;
}
