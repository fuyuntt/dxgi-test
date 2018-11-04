// dxgi-test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "duplication_manager.h"
#include "time.h"

#define OUTPUT 0

int main()
{
	DuplicationManager* duplication_manager = new DuplicationManager();
	DuplReturn hr;
	hr = duplication_manager->Init(OUTPUT);
	if (hr != DUPL_RETURN_SUCCESS)
	{
		DisplayMsg(L"��ʼ��ʧ��", L"MSG", 0);
		return 1;
	}
	bool is_timeout;
	FrameData frame_data;
	time_t start_sec = time(NULL);
	time_t end_sec = start_sec + 360;
	int count = 0;
	while (time(NULL) < end_sec)
	{
		hr = duplication_manager->GetFrame(10, &frame_data, &is_timeout);
		if (hr == DUPL_RETURN_ERROR_EXPECTED)
		{
			printf("ϵͳ����˲̬����ʼ���³�ʼ��\n");
			duplication_manager->DoneWithFrame();
			delete duplication_manager;
			duplication_manager = new DuplicationManager();
			hr = duplication_manager->Init(OUTPUT);
			if (hr != DUPL_RETURN_SUCCESS)
			{
				delete duplication_manager;
				printf("��ʼ��ʧ��\n");
				return 1;
			}
			printf("��ʼ���ɹ�\n");
			continue;
		}
		else if (hr == DUPL_RETURN_ERROR_UNEXPECTED)
		{
			DisplayMsg(L"��ȡ����ʧ��-����ʧ��", L"MSG", 0);
			return 2;
		}
		count++;
		if (!is_timeout)
		{
			char path[30];
			sprintf_s(path, "d:\\temp\\test-%d.png", count);
			WritePng(frame_data.buffer, frame_data.width, frame_data.height, path);
			Sleep(2000);
		}
		duplication_manager->DoneWithFrame();
	}
	wchar_t msg[100];
	swprintf_s(msg, L"������ͼ%d��, ��Լ%d��ÿ��", count, count / 20);
	DisplayMsg(msg, L"Msg", 0);
    return 0;
}
