#ifndef LOGGER_H_
#define LOGGER_H_

#include <mutex>
#include <string>
namespace log4c
{
	class Logger
	{
	private:
		std::mutex mutex_;
		std::string logger_name_;
		void PrintInfo(const char* level, unsigned short color, const char* format, va_list args);
	public:
		Logger(std::string logger_name)
		{
			logger_name_ = logger_name;
		}
		void Debug(const char* format, ...);
		void Info(const char* format, ...);
		void Error(const char* format, ...);
	};
}
#endif