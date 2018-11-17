#include "stdafx.h"
#include "logger.h"
#include <thread>
#include <iostream>

static log4c::Logger logger("main_test");
int main_bak()
{
	//thread t(thread_task);
	//t.join();
	logger.Info("this is a info");
	logger.Error("this is a error");
	logger.Debug("this is a info");
	system("pause");
	return EXIT_SUCCESS;
}