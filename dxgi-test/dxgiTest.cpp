// dxgi-test.cpp : �������̨Ӧ�ó������ڵ㡣
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
		DisplayMsg(L"��ʼ��ʧ��", L"MSG", 0);
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
