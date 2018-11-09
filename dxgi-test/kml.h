#ifndef GAME_KML_H_
#define GAME_KML_H_

#include <windows.h>

namespace kml
{
	BOOL LoadDLL();
	BOOL InitDevice();
	BOOL KeyDown(LPCSTR key);
	BOOL KeyUp(LPCSTR key);
	BOOL KeyPress(LPCSTR key, int count);
	BOOL LeftUp();
	BOOL LeftDown();
	BOOL LeftClick(int count);
	BOOL RightUp();
	BOOL RightDown();
	BOOL RightClick(int count);
	BOOL WheelsMove(int y);
}
#endif
