#include "stdafx.h"
#include "logger.h"
#include "stdio.h"
#include "stdarg.h"
#include "ctime"

namespace logger
{
	void print_time();

	void info(const char* format, ...)
	{
		print_time();
		printf(" [INFO] ");
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);
		printf("\n");
	}

	void error(const char* format, ...)
	{
		print_time();
		printf(" [ERROR] ");
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);
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