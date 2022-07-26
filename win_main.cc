#include <Windows.h>
#include <tchar.h>

HANDLE g_hmutex;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR lpCmdLine, int cmd_show) {
	HANDLE g_hmutex = CreateMutex(NULL, FALSE, TEXT("{FA531CC1-0497-11d3-A180-00105A276C3E}"));
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, TEXT("App already run"), TEXT("Message"), MB_OK);
		CloseHandle(g_hmutex);
		return 0;
	}

	WNDCLASSEX wndclass;
	TCHAR class_name[] = TEXT("FirstWindow");
	TCHAR app_name[] = TEXT("Hello Win32");
	HWND hwnd;
	MSG msg;

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = instance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = class_name;
	wndclass.hIconSm = NULL;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(0, class_name, app_name, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		300, 100, NULL, NULL, instance, NULL);
	
	ShowWindow(hwnd, cmd_show);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	HDC hdc;
	PAINTSTRUCT ps;
	TCHAR str[] = TEXT("Hello, Win32");
	switch (msg) {
	case WM_CREATE:
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		TextOut(hdc, 10, 10, str, _tcslen(str));
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		CloseHandle(g_hmutex);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}