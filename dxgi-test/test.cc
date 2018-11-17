#include "stdafx.h"

#include "logger.h"

#include <thread>
#include <iostream>

using namespace std;
void thread_task()
{
	cout << "thread task start" << endl;
}
int main()
{
	//thread t(thread_task);
	//t.join();
	logger::info("this is a info");
	logger::error("this is a error");
	logger::info("this is a info");
	system("pause");
	return EXIT_SUCCESS;
}