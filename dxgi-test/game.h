#ifndef DXGI_GAME_H_
#define DXGI_GAME_H_

#include "unordered_map"
#include "duplication_manager.h"
#include "logger.h"

namespace game {
	bool HasNext(const RECT& rect, const POINT& p);
	void Next(const RECT& rect, POINT* p);
	void SaveFrameAsPng(dupl::FrameData* frame, const unsigned hash);
	class Context
	{
	public:
		inline void put(std::string key, void* value_ptr) {
			map.insert(std::make_pair(key, value_ptr));
		}
		void* get(std::string key)
		{
			std::unordered_map<std::string, void*>::iterator iter = map.find(key);
			if (iter == map.end()) {
				logger::error("找不到要查找的元素");
				return NULL;
			}
			return iter->second;
		}
	private:
		std::unordered_map<std::string, void*> map;
	};

	class Filter
	{
	public:
		virtual void Run(dupl::FrameData* frame_data, Context* context) = 0;
	};

	class Controller
	{
	public:
		Controller();
		~Controller();
		ReturnStatus Init();
		void AddFilter(Filter* filter);
		ReturnStatus StartGaming();

	private:
		dupl::DuplicationManager* dupl_manager_;
		std::vector<Filter*> filters_;
	};

}

#endif // DXGI_GAME_H_
