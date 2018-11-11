#include "stdafx.h"

#include "filters.h"
#include "kml_manager.h"

namespace game
{
	static const RECT kSnipRect = { 470, 385, 472, 385 };
	static const dupl::COLOR_BGRA kBlack = { 0x00, 0x00, 0x00, 0xff };
	bool SniperFilter::Run(dupl::FrameData* frame_data, Context* context)
	{
		switch (state_)
		{
		case NON_SNIPE:
			return NonSnipeRun(frame_data, context);
			break;
		case SNIPE:
			return SnipeRun(frame_data, context);
			break;
		case SLEEPING:
			return SleepRun(frame_data, context);
			break;
		default:
			logger::error("不应运行到这里");
			break;
		}
		return true;
	}
	bool SniperFilter::NonSnipeRun(dupl::FrameData* frame_data, Context* context)
	{
		if (context->weapon_type == SNIPER)
		{
			state_ = SNIPE;
		} 
		return state_ != SNIPE;
	}
	bool isRoom(dupl::FrameData* frame_data);
	bool SniperFilter::SnipeRun(dupl::FrameData* frame_data, Context* context)
	{
		if (context->weapon_type != SNIPER)
		{
			state_ = NON_SNIPE;
			return true;
		} 
		if (context->is_in_range && isRoom(frame_data))
		{
			PostThreadMessage(kml::thread_id, KML_LEFT_CLICK, 0, 1);
			PostThreadMessage(kml::thread_id, KML_SLEEP, 0, 150);
			PostThreadMessage(kml::thread_id, KML_WHEELS_MOVES, 0, -1);
			weekup_tick = context->tick_count + 650;
			SHORT capital_state = GetKeyState(VK_CAPITAL) & 0x01;
			if (!capital_state)
			{
				PostThreadMessage(kml::thread_id, KML_SLEEP, 0, 200);
				PostThreadMessage(kml::thread_id, KML_WHEELS_MOVES, 0, 1);
				weekup_tick += 200;
			}
			state_ = SLEEPING;
		}
		return false;
	}

	bool SniperFilter::SleepRun(dupl::FrameData* frame_data, Context* context)
	{
		if (context->tick_count > weekup_tick)
		{
			state_ = SNIPE;
		}
		return false;
	}

	bool isRoom(dupl::FrameData* frame_data)
	{
		for (POINT p = BeginPoint(kSnipRect); IsInRect(kSnipRect, p); Next(kSnipRect, &p))
		{
			dupl::COLOR_BGRA* pixel = frame_data->GetPixel(p);
			if (*pixel != kBlack)
				return false;
		}
		return true;
	}
}
