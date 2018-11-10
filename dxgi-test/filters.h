#ifndef GAME_FILTERS_H_
#define GAME_FILTERS_H_

#include "game.h"
namespace game
{
	class WeaponFilter : public Filter
	{
	public:
		bool Run(dupl::FrameData* frame_data, Context* context);
	};

	class IsInRangeFilter : public Filter
	{
	public:
		bool Run(dupl::FrameData* frame_data, Context* context);
	};

	class SniperFilter : public Filter
	{
	public:
		bool Run(dupl::FrameData* frame_data, Context* context);
	};
}

#endif