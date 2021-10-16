#include "stdafx.h"

#include "keyboard_mouse.h"
#include "logger.h"
#include <Windows.h>
#include <memory>

namespace km
{
	typedef int(__stdcall *OpenDeviceFunc)(char* SerialNumber);
	typedef int(__stdcall *KeyDownFunc)(LPCSTR key);
	typedef int(__stdcall *KeyUpFunc)(LPCSTR key);
	typedef int(__stdcall *KeyPressFunc)(LPCSTR key);
	typedef int(__stdcall* MouseDownFunc)(int mButton);
	typedef int(__stdcall* MouseUpFunc)(int mButton);
	typedef int(__stdcall* MouseClickFunc)(int mButton);
	typedef int(__stdcall *WheelsMoveFunc)(int y);
	static OpenDeviceFunc openDevice;
	static KeyDownFunc keyDown;
	static KeyUpFunc keyUp;
	static KeyPressFunc keyPress;
	static MouseDownFunc mouseDown;
	static MouseUpFunc mouseUp;
	static MouseClickFunc mouseClick;
	static WheelsMoveFunc wheelsMove;

	bool KeyboardMouse::Init()
	{
		logger_.Info("开始载入键鼠动态库");
		HINSTANCE hDllInst = LoadLibrary(L"kmllib.dll");
		if (!hDllInst)
		{
			logger_.Error("键鼠动态库载入失败");
			return false;
		}
		openDevice = (OpenDeviceFunc)GetProcAddress(hDllInst, "SelectDeviceBySerialNumber");
		keyDown = (KeyDownFunc)GetProcAddress(hDllInst, "PressKeyByName");
		keyUp = (KeyUpFunc)GetProcAddress(hDllInst, "ReleaseKeyByName");
		keyPress = (KeyPressFunc)GetProcAddress(hDllInst, "PressAndReleaseKeyByName");
		mouseDown = (MouseDownFunc)GetProcAddress(hDllInst, "PressMouseButton");
		mouseUp = (MouseUpFunc)GetProcAddress(hDllInst, "ReleaseMouseButton");
		mouseClick = (MouseClickFunc)GetProcAddress(hDllInst, "PressAndReleaseMouseButton");
		wheelsMove = (WheelsMoveFunc)GetProcAddress(hDllInst, "MoveMouseWheel");
		logger_.Info("键鼠动态库载入完成");
		logger_.Info("开始初始化设备");
		if (!openDevice(NULL))
		{
			logger_.Error("初始化键鼠设备失败\n");
			return false;
		}
		logger_.Info("初始化键鼠设备成功\n");
		logger_.Info("开始键鼠操作线程");
		std::thread thread(&KeyboardMouse::DealMessage, this);
		thread.detach();
		logger_.Info("键鼠操作线程启动完成");
		return true;
	}

	void KeyboardMouse::DealMessage()
	{
		logger_.Info("开始监听键鼠消息");
		while (true)
		{
			std::shared_ptr<Message> kml_msg = message_queue_.take();
			logger_.Debug("收到消息[%d], 内容[%s], 计数[%d]", kml_msg->msg, kml_msg->content.c_str(), kml_msg->count);
			std::string content = kml_msg->content;
			switch (kml_msg->msg)
			{
			case KML_KEY_DOWN:
				keyDown(content.c_str());
				break;
			case KML_KEY_UP:
				keyUp(content.c_str());
				break;
			case KML_KEY_PRESS:
				keyPress(content.c_str());
				break;
			case KML_LEFT_DOWN:
				mouseDown(1);
				break;
			case KML_LEFT_UP:
				mouseUp(1);
				break;
			case KML_LEFT_CLICK:
				mouseClick(1);
				break;
			case KML_RIGHT_DOWN:
				mouseDown(3);
				break;
			case KML_RIGHT_UP:
				mouseUp(3);
				break;
			case KML_RIGHT_CLICK:
				mouseClick(3);
				break;
			case KML_WHEELS_MOVES:
				wheelsMove(kml_msg->count);
				break;
			case KML_SLEEP:
				Sleep(kml_msg->count);
				break;
			default:
				logger_.Error("未知消息[%d], 内容[%s], 计数[%d]", kml_msg->msg, kml_msg->content, kml_msg->count);
				break;
			}
		}
		logger_.Error("键鼠线程结束");
	}

	void KeyboardMouse::SendMsg(MessageType msg, std::string content, long count)
	{
		message_queue_.put(std::shared_ptr<Message>(new Message{ msg, content, count }));
	}
}