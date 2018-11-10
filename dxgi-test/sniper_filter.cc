#include "stdafx.h"

#include "filters.h"
namespace game
{
	bool SniperFilter::Run(dupl::FrameData* frame_data)
	{
		if (WeaponFilter::weapon_type_ != SNIPER)
		{
			return true;
		}

		return true;
	}
}
