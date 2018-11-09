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
		logger::info("��ʼ�������̬��");
		HINSTANCE hDllInst = LoadLibrary(L"kmllib.dll");
		if (!hDllInst)
		{
			logger::error("����̬������ʧ��");
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