#include "stdafx.h"

#include "filters.h"
namespace game
{
	bool SniperFilter::Run(dupl::FrameData* frame_data, Context* context)
	{
		if (context->weapon_type != SNIPER)
		{
			return true;
		}

		return true;
	}
}
