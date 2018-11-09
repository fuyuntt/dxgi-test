#include "stdafx.h"
#include "kml.h"
#include "logger.h"

namespace kml
{
	static int(*openDevice)();
	static int(*keyDown)(LPCSTR key);
	static int(*keyUp)(LPCSTR key);
	static int(*keyPress)(LPCSTR key, int count);
	static int(*leftDown)();
	static int(*leftUp)();
	static int(*leftClick)(int count);
	static int(*rightDown)();
	static int(*rightUp)();
	static int(*rightClick)(int count);
	static int(*wheelsMove)(int y);

	BOOL LoadDLL()
	{
		logger::info("开始载入键鼠动态库");
		HINSTANCE hDllInst = LoadLibrary(L"kmllib.dll");
		if (!hDllInst)
		{
			logger::error("键鼠动态库载入失败");
			return false;
		}
		openDevice = (int(*)())GetProcAddress(hDllInst, "OpenDevice");
		keyDown = (int(*)(LPCSTR key))GetProcAddress(hDllInst, "KeyDown");
		keyUp = (int(*)(LPCSTR key))GetProcAddress(hDllInst, "KeyUp");
		keyPress = (int(*)(LPCSTR key, int count))GetProcAddress(hDllInst, "KeyPress");
		leftDown = (int(*)())GetProcAddress(hDllInst, "LeftDown");
		leftUp = (int(*)())GetProcAddress(hDllInst, "LeftUp");
		leftClick = (int(*)(int count))GetProcAddress(hDllInst, "LeftClick");
		rightDown = (int(*)())GetProcAddress(hDllInst, "rightDown");
		rightUp = (int(*)())GetProcAddress(hDllInst, "rightUp");
		rightClick = (int(*)(int count))GetProcAddress(hDllInst, "rightClick");
		wheelsMove = (int(*)(int y))GetProcAddress(hDllInst, "WheelsMove");
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