#include "stdafx.h"
#include "filters.h"
#include "png_writer.h"

namespace game
{
	static RECT weapon_name_rect = { 816, 695, 1000, 704 };
	static const dupl::COLOR_BGRA kWeaponNameColor = { 0xC1, 0xC1, 0x99, 0xff };

	struct Weapon
	{
		unsigned hash;
		const char* weapon_name;
		WeaponType weapon_type;
	};
	static const std::unordered_map<unsigned, Weapon*>::value_type init_value[] =
	{
		std::unordered_map<unsigned, Weapon*>::value_type(123, new Weapon {23, "哈哈", SNIPER})
	};
	static const std::unordered_map<unsigned, Weapon*> weapons(init_value, init_value + 1);
	static std::unordered_map<unsigned, void*> saved_hash;

	void WeaponFilter::Run(dupl::FrameData* frame_data, Context* context)
	{
		if (frame_data == NULL)
		{
			return;
		}
		context->put(kWeapon, &weapon_type_);
		unsigned hash = 0;
		POINT p = { weapon_name_rect.left, weapon_name_rect.top };
		while (HasNext(weapon_name_rect, p))
		{
			dupl::COLOR_BGRA* color_ptr = frame_data->GetPixel(p.x, p.y);
			if (*color_ptr == kWeaponNameColor)
			{
				hash = 31 * hash + p.x;
				hash = 31 * hash + p.y;
			}
			Next(weapon_name_rect, &p);
		}
		std::unordered_map<unsigned, Weapon*>::const_iterator it = weapons.find(hash);
		if (it == weapons.end())
		{
			if (saved_hash.find(hash) == saved_hash.end())
			{
				logger::info("找不到该hash的枪,开始使用默认的选项[步枪]");
				SaveFrameAsPng(frame_data, hash);
				saved_hash.insert(std::pair<unsigned, void*>(hash, NULL));
			}
			weapon_type_ = AUTOMATIC_RIFLE;
			return;
		}
		Weapon* weapon = it->second;
		logger::info("武器切换到[%s]， 类型[%d]", weapon->weapon_name, weapon->weapon_type);
	}
}