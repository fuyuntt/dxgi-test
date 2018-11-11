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
		logger::info("开始载入键鼠动态库");
		HINSTANCE hDllInst = LoadLibrary(L"kmllib.dll");
		if (!hDllInst)
		{
			logger::error("键鼠动态库载入失败");
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
		logger::info("键鼠动态库载入完成");
		return true;
	}

	// 先载入动态库 再初始化设备
	BOOL InitDevice()
	{
		logger::info("开始初始化设备");
		if (!openDevice())
		{
			logger::error("初始化设备失败\n");
			return false;
		}
		logger::info("初始化设备成功\n");
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