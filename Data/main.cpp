//
//  main.cpp
//  BINS_workstation
//
//  Created by Denis Fedorov on 02.02.2023.
//

#if defined(_WIN32)
#include <Windows.h>
#endif
#include "interface.h"

int main()
{
#if defined(_WIN32)
//	auto console {GetConsoleWindow()};
//	RECT rect {};
//	GetWindowRect(console, &rect);
//	MoveWindow(console, rect.left, rect.top, 900, 500, true);
	COORD coord {};
	coord.X = 100;
	coord.Y = 9999;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	SetConsoleTitleA("BINS_workstation: DATA");
	SetConsoleCP(1251);
	SetConsoleOutputCP(65001);
#endif
	ws::data::interface interface;
	interface.start();
	return 0;
}
