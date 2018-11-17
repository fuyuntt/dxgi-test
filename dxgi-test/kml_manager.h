#ifndef KML_MANAGER_H__
#define KML_MANAGER_H__

#include <memory>
#include "blocking_queue.h"

namespace kml
{
	enum MessageType
	{
		KML_KEY_DOWN,
		KML_KEY_UP,
		KML_KEY_PRESS,
		KML_LEFT_DOWN,
		KML_LEFT_UP,
		KML_LEFT_CLICK,
		KML_RIGHT_DOWN,
		KML_RIGHT_UP,
		KML_RIGHT_CLICK,
		KML_WHEELS_MOVES,
		KML_SLEEP
	};
	struct Message
	{
		MessageType msg;
		std::string content;
		int count;
	};
	void StartKmlManager(BlockingQueue<std::shared_ptr<Message>>& msg_queue);
}

#endif // !KML_MANAGER_H__

