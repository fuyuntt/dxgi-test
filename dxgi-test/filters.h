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
		const std::string kWeapon = "weapon";
		void Run(dupl::FrameData* frame_data, Context* context);
	private:
		WeaponType weapon_type_;
	};
}

#endif