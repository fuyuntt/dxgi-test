// dxgi-test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "duplication_manager.h"
#include "time.h"

int main()
{
	DuplicationManager* pMgr = new DuplicationManager();
	DuplReturn hr;
	hr = pMgr->InitDupl(0);
	if (hr != DUPL_RETURN_SUCCESS)
	{
		DisplayMsg(L"��ʼ��ʧ��", L"MSG", 0);
		return 1;
	}
	bool is_timeout;
	FrameData FrameData;
	time_t startSec = time(NULL);
	time_t endSec = startSec + 20;
	int count = 0;
	while (time(NULL) < endSec)
	{
		hr = pMgr->GetFrame(10, &FrameData, &is_timeout);
		if (hr != DUPL_RETURN_SUCCESS)
		{
			DisplayMsg(L"��ȡ����ʧ��", L"MSG", 0);
			return 2;
		}
		count++;
	}
	wchar_t msg[100];
	swprintf_s(msg, L"������ͼ%d��, ��Լ%d��ÿ��", count, count / 20);
	DisplayMsg(msg, L"Msg", 0);
    return 0;
}
