// dxgi-test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "duplication_manager.h"
#include "time.h"

int main()
{
	DuplicationManager* duplication_manager = new DuplicationManager();
	DuplReturn hr;
	hr = duplication_manager->Init(0);
	if (hr != DUPL_RETURN_SUCCESS)
	{
		DisplayMsg(L"初始化失败", L"MSG", 0);
		return 1;
	}
	bool is_timeout;
	FrameData frame_data;
	time_t start_sec = time(NULL);
	time_t end_sec = start_sec + 20;
	int count = 0;
	while (time(NULL) < end_sec)
	{
		hr = duplication_manager->GetFrame(10, &frame_data, &is_timeout);
		if (hr != DUPL_RETURN_SUCCESS)
		{
			DisplayMsg(L"获取数据失败", L"MSG", 0);
			return 2;
		}
		count++;
		if (is_timeout)
		{
			continue;
		}
		duplication_manager->DoneWithFrame();
	}
	wchar_t msg[100];
	swprintf_s(msg, L"创建截图%d个, 大约%d个每秒", count, count / 20);
	DisplayMsg(msg, L"Msg", 0);
    return 0;
}
