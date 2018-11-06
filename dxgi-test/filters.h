#ifndef GAME_FILTERS_H_
#define GAME_FILTERS_H_

#include "game.h"
namespace game
{
	class WeaponFilter : public Filter
	{
		void Run(const dupl::FrameData* frameData, Context* context);
	};
}

#endif