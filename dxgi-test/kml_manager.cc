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
		logger::info("开始监听键鼠消息");
		MSG msg;
		char* key = "-";
		BOOL msg_ret;
		while ((msg_ret = GetMessage(&msg, NULL, WM_USER, WM_USER + 16)) != 0)
		{
			if (msg_ret == -1)
			{
				logger::error("消息队列异常 %d", msg_ret);
				continue;
			}
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
				logger::info("left click %ld", msg.lParam);
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
				logger::info("wheels move %ld", msg.lParam);
				break;
			case KML_SLEEP:
				logger::info("sleep %ld", msg.lParam);
				Sleep(msg.lParam);
				logger::info("sleep %ld end", msg.lParam);
				break;
			default:
				break;
			}
		}
		logger::info("键鼠线程结束");
		return 0;
	}
	unsigned int thread_id;
	bool StartThread()
	{
		unsigned res = _beginthreadex(NULL, 0, kml::ManageKml, NULL, 0, &thread_id);
		if (res == 0)
		{
			logger::error("键鼠操作线程创建失败");
			return false;
		}
		return true;
	}
}