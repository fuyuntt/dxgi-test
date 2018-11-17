#include "stdafx.h"
#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <ctime>
#include <mutex>
#include <Windows.h>

namespace log
{
	static void print_time();
	static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	static const WORD kForegroundWhite = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	static const WORD kForegroundGrey = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	static const WORD kForegroundRed = FOREGROUND_INTENSITY | FOREGROUND_RED;
	static const WORD kBackgroundBlack = 0;

	void Logger::Info(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		PrintInfo("INFO", kForegroundWhite, format, args);
	}

	void Logger::Debug(const char* format, ...)
	{
#ifdef DEBUG
		va_list args;
		va_start(args, format);
		PrintInfo("DEBUG", kForegroundGrey, format, args);
#endif // DEBUG
	}

	void Logger::Error(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		PrintInfo("ERROR", kForegroundRed, format, args);
	}

	void Logger::PrintInfo(const char* level, unsigned short color, const char* format, va_list args)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		SetConsoleTextAttribute(handle, color | kBackgroundBlack);
		print_time();
		printf(" [%s] %s: ", level, logger_name_.c_str());
		vprintf(format, args);
		printf("\n");
	}
	static void print_time()
	{
		std::time_t now = std::time(nullptr);
		char time_str[21];
		std::strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", std::localtime(&now));
		printf(time_str);
	}
}