#include "stdafx.h"

#include "keyboard_mouse.h"
#include "logger.h"
#include <Windows.h>
#include <memory>

namespace km
{
	typedef int(__stdcall *OpenDeviceFunc)();
	typedef int(__stdcall *KeyDownFunc)(LPCSTR key);
	typedef int(__stdcall *KeyUpFunc)(LPCSTR key);
	typedef int(__stdcall *KeyPressFunc)(LPCSTR key, int count);
	typedef int(__stdcall *LeftDownFunc)();
	typedef int(__stdcall *LeftUpFunc)();
	typedef int(__stdcall *LeftClickFunc)(int count);
	typedef int(__stdcall *RightDownFunc)();
	typedef int(__stdcall *RightUpFunc)();
	typedef int(__stdcall *RightClickFunc)(int count);
	typedef int(__stdcall *WheelsMoveFunc)(int y);
	static OpenDeviceFunc openDevice;
	static KeyDownFunc keyDown;
	static KeyUpFunc keyUp;
	static KeyPressFunc keyPress;
	static LeftDownFunc leftDown;
	static LeftUpFunc leftUp;
	static LeftClickFunc leftClick;
	static RightDownFunc rightDown;
	static RightUpFunc rightUp;
	static RightClickFunc rightClick;
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
		openDevice = (OpenDeviceFunc)GetProcAddress(hDllInst, "OpenDevice");
		keyDown = (KeyDownFunc)GetProcAddress(hDllInst, "KeyDown");
		keyUp = (KeyUpFunc)GetProcAddress(hDllInst, "KeyUp");
		keyPress = (KeyPressFunc)GetProcAddress(hDllInst, "KeyPress");
		leftDown = (LeftDownFunc)GetProcAddress(hDllInst, "LeftDown");
		leftUp = (LeftUpFunc)GetProcAddress(hDllInst, "LeftUp");
		leftClick = (LeftClickFunc)GetProcAddress(hDllInst, "LeftClick");
		rightDown = (RightDownFunc)GetProcAddress(hDllInst, "RightDown");
		rightUp = (RightUpFunc)GetProcAddress(hDllInst, "RightUp");
		rightClick = (RightClickFunc)GetProcAddress(hDllInst, "RightClick");
		wheelsMove = (WheelsMoveFunc)GetProcAddress(hDllInst, "WheelsMove");
		logger_.Info("键鼠动态库载入完成");
		logger_.Info("开始初始化设备");
		if (!openDevice())
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
				keyPress(content.c_str(), kml_msg->count);
				break;
			case KML_LEFT_DOWN:
				leftDown();
				break;
			case KML_LEFT_UP:
				leftUp();
				break;
			case KML_LEFT_CLICK:
				leftClick(kml_msg->count);
				break;
			case KML_RIGHT_DOWN:
				rightDown();
				break;
			case KML_RIGHT_UP:
				rightUp();
				break;
			case KML_RIGHT_CLICK:
				rightClick(kml_msg->count);
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