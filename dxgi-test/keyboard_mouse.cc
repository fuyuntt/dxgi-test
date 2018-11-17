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
		logger_.Info("��ʼ�������̬��");
		HINSTANCE hDllInst = LoadLibrary(L"kmllib.dll");
		if (!hDllInst)
		{
			logger_.Error("����̬������ʧ��");
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
		logger_.Info("����̬���������");
		logger_.Info("��ʼ��ʼ���豸");
		if (!openDevice())
		{
			logger_.Error("��ʼ�������豸ʧ��\n");
			return false;
		}
		logger_.Info("��ʼ�������豸�ɹ�\n");
		logger_.Info("��ʼ��������߳�");
		std::thread thread(&KeyboardMouse::DealMessage, this);
		thread.detach();
		logger_.Info("��������߳��������");
		return true;
	}

	void KeyboardMouse::DealMessage()
	{
		logger_.Info("��ʼ����������Ϣ");
		while (true)
		{
			std::shared_ptr<Message> kml_msg = message_queue_.take();
			logger_.Debug("�յ���Ϣ[%d], ����[%s], ����[%d]", kml_msg->msg, kml_msg->content.c_str(), kml_msg->count);
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
				logger_.Error("δ֪��Ϣ[%d], ����[%s], ����[%d]", kml_msg->msg, kml_msg->content, kml_msg->count);
				break;
			}
		}
		logger_.Error("�����߳̽���");
	}

	void KeyboardMouse::SendMsg(MessageType msg, std::string content, long count)
	{
		message_queue_.put(std::shared_ptr<Message>(new Message{ msg, content, count }));
	}
}