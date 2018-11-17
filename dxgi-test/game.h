#ifndef DXGI_GAME_H_
#define DXGI_GAME_H_

#include "unordered_map"
#include "duplication_manager.h"
#include "keyboard_mouse.h"
#include "png_writer.h"
#include "logger.h"

namespace game {
	POINT BeginPoint(const RECT& rect);
	bool IsInRect(const RECT& rect, const POINT& p);
	void Next(const RECT& rect, POINT* p);

	enum WeaponType {
		NONE,
		SNIPER,
		AUTOMATIC_RIFLE
	};

	struct Context
	{
		km::KeyboardMouse* keyboard_mouse;
		png::PngWriter* png_writer;
		WeaponType weapon_type;
		bool is_in_range;
		DWORD tick_count;
	};

	class Filter
	{
	public:
		virtual bool Run(dupl::FrameData* frame_data, Context* context) = 0;
	};

	class Controller
	{
	public:
		Controller();
		~Controller();
		ReturnStatus Init(std::string picture_dir);
		void AddFilter(Filter* filter);
		ReturnStatus StartGaming();

	private:
		dupl::DuplicationManager* dupl_manager_;
		km::KeyboardMouse* keyboard_mouse_;
		png::PngWriter* png_writer_;
		std::vector<Filter*> filters_;
	};

}

#endif // DXGI_GAME_H_
