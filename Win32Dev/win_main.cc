#include <Windows.h>
#include <tchar.h>
#include <xstring>
#include <iostream>

#include "gapp.h"
using namespace std;

#pragma comment(lib,"ComCtl32.lib") 

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR lpCmdLine, int cmd_show) {
	GApp app;
	return app.Run();
}

