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
		logger_.Info("��ʼ�������̬��");
		HINSTANCE hDllInst = LoadLibrary(L"kmllib.dll");
		if (!hDllInst)
		{
			logger_.Error("����̬������ʧ��");
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
		logger_.Info("����̬���������");
		logger_.Info("��ʼ��ʼ���豸");
		if (!openDevice(NULL))
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