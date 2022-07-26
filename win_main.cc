#include <Windows.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR lpCmdLine, int cmd_show) {

	MessageBox(NULL, TEXT("Hello World"), TEXT("First App"), MB_OK);

	return 0;
}