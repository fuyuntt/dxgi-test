#include "stdafx.h"
#include "kml.h"
#include "logger.h"

namespace kml
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
	OpenDeviceFunc openDevice;
	KeyDownFunc keyDown;
	KeyUpFunc keyUp;
	KeyPressFunc keyPress;
	LeftDownFunc leftDown;
	LeftUpFunc leftUp;
	LeftClickFunc leftClick;
	RightDownFunc rightDown;
	RightUpFunc rightUp;
	RightClickFunc rightClick;
	WheelsMoveFunc wheelsMove;

	BOOL LoadDLL()
	{
		logger::info("��ʼ�������̬��");
		HINSTANCE hDllInst = LoadLibrary(L"kmllib.dll");
		if (!hDllInst)
		{
			logger::error("����̬������ʧ��");
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
		logger::info("����̬���������");
		return true;
	}

	// �����붯̬�� �ٳ�ʼ���豸
	BOOL InitDevice()
	{
		logger::info("��ʼ��ʼ���豸");
		if (!openDevice())
		{
			logger::error("��ʼ���豸ʧ��\n");
			return false;
		}
		logger::info("��ʼ���豸�ɹ�\n");
		return true;
	}

	BOOL KeyDown(LPCSTR key)
	{
		return keyDown(key);
	}
	BOOL KeyUp(LPCSTR key)
	{
		return keyUp(key);
	}
	BOOL KeyPress(LPCSTR key, int count)
	{
		return keyPress(key, count);
	}
	BOOL LeftUp()
	{
		return leftUp();
	}
	BOOL LeftDown()
	{
		return leftDown();
	}
	BOOL LeftClick(int count)
	{
		return leftClick(count);
	}
	BOOL RightUp()
	{
		return rightUp();
	}
	BOOL RightDown()
	{
		return rightDown();
	}
	BOOL RightClick(int count)
	{
		return rightClick(count);
	}
	BOOL WheelsMove(int y)
	{
		return wheelsMove(y);
	}
}