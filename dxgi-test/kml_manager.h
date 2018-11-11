#ifndef KML_MANAGER_H__
#define KML_MANAGER_H__

#define KML_KEY_DOWN		0x0001
#define KML_KEY_UP			0x0002
#define KML_KEY_PRESS		0x0003
#define KML_LEFT_DOWN		0x0004
#define KML_LEFT_UP			0x0005
#define KML_LEFT_CLICK		0x0006
#define KML_RIGHT_DOWN		0x0007
#define KML_RIGHT_UP		0x0008
#define KML_RIGHT_CLICK		0x0009
#define KML_WHEELS_MOVES	0x000A
#define KML_SLEEP			0x000B

namespace kml
{
	extern unsigned int thread_id;
	unsigned __stdcall ManageKml(void* args);
	bool StartThread();
}

#endif // !KML_MANAGER_H__

