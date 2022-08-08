#include <Windows.h>
#include <tchar.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


HINSTANCE g_hinstance;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	TCHAR szClassName[] = TEXT("DDB_CLASS");
	TCHAR szAppName[] = TEXT("DDB_APP");
	MSG msg;
	HWND hwnd;

	g_hinstance = hInstance;

	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.lpszClassName = szClassName;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = 0;
	
	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(0, szClassName, szAppName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		400, 300, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc, hdcMem;
	HBITMAP hBitmap;
	BITMAP bmp;

	switch (uMsg) {
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		hdcMem = CreateCompatibleDC(hdc);
		hBitmap = LoadBitmap(g_hinstance, MAKEINTRESOURCE(IDB_GIRL));
		//hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Girl.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		SelectObject(hdcMem, hBitmap);

		// 绘图操作
		SetBkMode(hdcMem, TRANSPARENT);
		TextOut(hdcMem, 10, 10, TEXT("窈窕淑女 君子好逑"), _tcslen(TEXT("窈窕淑女 君子好逑")));

		// 把内存位图复制到窗口客户区中
		GetObject(hBitmap, sizeof(bmp), &bmp);
		BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcMem, 0, 0, SRCCOPY);

		EndPaint(hWnd, &ps);
		DeleteObject(hBitmap);
		DeleteDC(hdcMem);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}