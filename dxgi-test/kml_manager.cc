#include "stdafx.h"
#include "windows.h"
#include "kml_manager.h"
#include "kml.h"
#include "logger.h"
#include "blocking_queue.h"
#include <memory>
#include <thread>

namespace kml
{
	void MessageExecutor(BlockingQueue<std::shared_ptr<Message>>& msg_queue)
	{
		logger::info("开始监听键鼠消息");
		while (true)
		{
			std::shared_ptr<Message> kml_msg = msg_queue.take();
			std::string content = kml_msg->content;
			switch (kml_msg->msg)
			{
			case KML_KEY_DOWN:
				KeyDown(content.c_str());
				break;
			case KML_KEY_UP:
				KeyUp(content.c_str());
				break;
			case KML_KEY_PRESS:
				KeyPress(content.c_str(), kml_msg->count);
				break;
			case KML_LEFT_DOWN:
				LeftDown();
				break;
			case KML_LEFT_UP:
				LeftUp();
				break;
			case KML_LEFT_CLICK:
				LeftClick(kml_msg->count);
				logger::info("left click [%ld]", kml_msg->count);
				break;
			case KML_RIGHT_DOWN:
				RightDown();
				break;
			case KML_RIGHT_UP:
				RightUp();
				break;
			case KML_RIGHT_CLICK:
				RightClick(kml_msg->count);
				break;
			case KML_WHEELS_MOVES:
				logger::info("wheels move [%ld]", kml_msg->count);
				WheelsMove(kml_msg->count);
				break;
			case KML_SLEEP:
				Sleep(kml_msg->count);
				logger::info("sleep [%ld] end", kml_msg->count);
				break;
			default:
				logger::info("unknown message [%d]", kml_msg->msg);
				break;
			}
		}
		logger::info("键鼠线程结束");
	}
	void StartKmlManager(BlockingQueue<std::shared_ptr<Message>>& msg_queue)
	{
		std::thread kml_thread(MessageExecutor, msg_queue);
		kml_thread.detach();
	}
}