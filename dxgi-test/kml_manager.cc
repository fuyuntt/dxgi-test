#include "stdafx.h"
#include "windows.h"
#include "kml_manager.h"
#include "kml.h"
#include "logger.h"
#include "process.h"

namespace kml
{
	unsigned __stdcall ManageKml(void* args)
	{
		logger::info("��ʼ����������Ϣ");
		MSG msg;
		char* key = "-";
		while (GetMessage(&msg, 0, 0, 0))
		{
			switch (msg.message)
			{
			case KML_KEY_DOWN:
				key[0] = msg.wParam;
				KeyDown(key);
				break;
			case KML_KEY_UP:
				key[0] = msg.wParam;
				KeyUp(key);
				break;
			case KML_KEY_PRESS:
				key[0] = msg.wParam;
				KeyPress(key, msg.lParam);
				break;
			case KML_LEFT_DOWN:
				LeftDown();
				break;
			case KML_LEFT_UP:
				LeftUp();
				break;
			case KML_LEFT_CLICK:
				LeftClick(msg.lParam);
				break;
			case KML_RIGHT_DOWN:
				RightDown();
				break;
			case KML_RIGHT_UP:
				RightUp();
				break;
			case KML_RIGHT_CLICK:
				RightClick(msg.lParam);
				break;
			case KML_WHEELS_MOVES:
				WheelsMove(msg.lParam);
				break;
			case KML_SLEEP:
				Sleep(msg.lParam);
				break;
			default:
				break;
			}
		}
		logger::info("�����߳̽���");
		return 0;
	}
	unsigned int thread_id;
	bool StartThread()
	{
		unsigned res = _beginthreadex(NULL, 0, kml::ManageKml, NULL, 0, &thread_id);
		if (res == 0)
		{
			logger::error("��������̴߳���ʧ��");
			return false;
		}
	}
}