#include "stdafx.h"
#include "filters.h"
#include "png_writer.h"

#define SCREEN_WIDTH 1024

namespace game
{
	// 武器名称
	static const RECT weapon_name_rect = { 816, 695, 1000, 704 };
	// 准星
	static const RECT front_sight = { 380, 384, 511, 384 };

	static const dupl::COLOR_BGRA kWeaponNameColor = { 0xC1, 0xC1, 0x99, 0xff };

	struct Weapon
	{
		unsigned hash;
		const char* weapon_name;
		WeaponType weapon_type;
	};
	static const std::unordered_map<unsigned, Weapon*>::value_type init_value[] =
	{
		std::unordered_map<unsigned, Weapon*>::value_type(3925178, new Weapon {3925178, "TRG-21", SNIPER})
	};
	static const std::unordered_map<unsigned, Weapon*> weapons(init_value, init_value + 1);
	static std::unordered_map<unsigned, void*> saved_hash;

	static bool IsSniper(dupl::FrameData* frame_data)
	{
		int count = 0;
		for (POINT p = BeginPoint(front_sight); IsInRect(front_sight, p); Next(front_sight, &p))
		{
			if (*(frame_data->GetPixel(p)) == *(frame_data->GetPixel(p)))
			{
				count++;
			} else {
				count = 0;
			}
			if (count >= 4)
			{
				return false;
			}
		}
		return true;
	}

	bool WeaponFilter::Run(dupl::FrameData* frame_data, Context* context)
	{
		if (frame_data == NULL)
		{
			return true;
		}
		if (frame_data->width != SCREEN_WIDTH)
		{
			return false;
		}
		unsigned hash = 0;
		for (POINT p = BeginPoint(weapon_name_rect); IsInRect(weapon_name_rect, p); Next(weapon_name_rect, &p))
		{
			dupl::COLOR_BGRA* color_ptr = frame_data->GetPixel(p);
			if (*(frame_data->GetPixel(p)) == kWeaponNameColor)
			{
				hash = 31 * hash + p.x;
				hash = 31 * hash + p.y;
			}
		}
		if (!hash) {
			return false;
		}
		std::unordered_map<unsigned, Weapon*>::const_iterator it = weapons.find(hash);
		if (it == weapons.end())
		{
			context->weapon_type = IsSniper(frame_data) ? SNIPER : AUTOMATIC_RIFLE;
			if (saved_hash.find(hash) == saved_hash.end())
			{
				logger::info("找不到该hash的枪,开始使用默认的选项");
				png::SaveFrameAsPng(frame_data, hash);
				saved_hash.insert(std::pair<unsigned, void*>(hash, NULL));
			}
		} else {
			Weapon* weapon = it->second;
			context->weapon_type = weapon->weapon_type;
			logger::info("武器切换到[%s]， 类型[%d]", weapon->weapon_name, weapon->weapon_type);
		}
		return true;
	}
}