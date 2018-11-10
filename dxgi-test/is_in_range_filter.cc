#include "stdafx.h"
#include "filters.h"

namespace game
{
	const dupl::COLOR_BGRA kBlack = { 0x00, 0x00, 0x00, 0xff };
	const RECT kRectPeopleName = { 479, 425, 546, 440 };
	bool IsInRangeFilter::Run(dupl::FrameData* frame_data, Context* context)
	{
		if (frame_data == NULL)
		{
			return true;
		}
		int b_count = 0;
		int r_count = 0;
		for (POINT p = BeginPoint(kRectPeopleName); IsInRect(kRectPeopleName, p); Next(kRectPeopleName, &p))
		{
			dupl::COLOR_BGRA* pixel = frame_data->GetPixel(p);
			if (*pixel == kBlack) b_count++;
			if (pixel->b < 60 && pixel->g < 80 && pixel->r > 150) r_count++;
		}
		context->is_in_range = b_count > 15 && r_count > 20;
		return true;
	}
}