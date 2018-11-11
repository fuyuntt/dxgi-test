#ifndef KML_MANAGER_H__
#define KML_MANAGER_H__

const int KML_KEY_DOWN = WM_USER + 0x0001;
const int KML_KEY_UP = WM_USER + 0x0002;
const int KML_KEY_PRESS = WM_USER + 0x0003;
const int KML_LEFT_DOWN = WM_USER + 0x0004;
const int KML_LEFT_UP = WM_USER + 0x0005;
const int KML_LEFT_CLICK = WM_USER + 0x0006;
const int KML_RIGHT_DOWN=		WM_USER + 0x0007;
const int KML_RIGHT_UP	=	WM_USER + 0x0008;
const int KML_RIGHT_CLICK=		WM_USER + 0x0009;
const int KML_WHEELS_MOVES=	WM_USER + 0x000A;
const int KML_SLEEP		=	WM_USER + 0x000B;

namespace kml
{
	extern unsigned int thread_id;
	unsigned __stdcall ManageKml(void* args);
	bool StartThread();
}

#endif // !KML_MANAGER_H__

