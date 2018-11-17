#ifndef GAME_KML_H_
#define GAME_KML_H_

#include "blocking_queue.h"
#include "logger.h"

namespace km
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

	class KeyboardMouse
	{
	private:
		struct Message
		{
			MessageType msg;
			std::string content;
			long count;
		};
		BlockingQueue<std::shared_ptr<Message>> message_queue_;
		log::Logger logger_;
		void DealMessage();
	public:
		KeyboardMouse() : message_queue_(), logger_("KeyboardMouse") {};
		bool Init();
		void SendMsg(MessageType msg, std::string content, long count);
	};
}
#endif
