// dxgi-test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "DuplicationManager.h"
#include "time.h"

int main()
{
	DUPLICATIONMANAGER* pMgr = new DUPLICATIONMANAGER();
	DUPL_RETURN hr;
	hr = pMgr->InitDupl(0);
	if (hr != DUPL_RETURN_SUCCESS)
	{
		DisplayMsg(L"初始化失败", L"MSG", 0);
		return 1;
	}
	bool timeOut;
	FRAME_DATA FrameData;
	time_t startSec = time(NULL);
	time_t endSec = startSec + 20;
	int count = 0;
	while (time(NULL) < endSec)
	{
		hr = pMgr->GetFrame(&FrameData, &timeOut);
		if (hr != DUPL_RETURN_SUCCESS)
		{
			DisplayMsg(L"获取数据失败", L"MSG", 0);
			return 2;
		}
		count++;
	}
	wchar_t msg[100];
	swprintf_s(msg, L"创建截图%d个, 大约%d个每秒", count, count / 20);
	DisplayMsg(msg, L"Msg", 0);
    return 0;
}
