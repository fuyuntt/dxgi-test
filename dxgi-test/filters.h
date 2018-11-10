#ifndef GAME_FILTERS_H_
#define GAME_FILTERS_H_

#include "game.h"
namespace game
{
	enum WeaponType {
		SNIPER,
		AUTOMATIC_RIFLE
	};
	class WeaponFilter : public Filter
	{
	public:
		static WeaponType weapon_type_;
		bool Run(dupl::FrameData* frame_data);
	};

	class IsInRangeFilter : public Filter
	{
	public:
		static bool is_in_range_;
		bool Run(dupl::FrameData* frame_data);
	};

	class SniperFilter : public Filter
	{
	public:
		bool Run(dupl::FrameData* frame_data);
	};

	
}

#endif