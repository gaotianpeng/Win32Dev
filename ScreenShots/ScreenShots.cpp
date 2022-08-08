#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include "resource.h"

BOOL g_bCapturing;
INT g_nLBtnDownCount;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 第1次鼠标按下，画十字线
VOID DrawCrossLine(HWND hWnd, POINT ptBegin, int cxScreen, int cyScreen, HDC hdcMem);

// 第2次鼠标按下, 画矩形
VOID DrawRect(HWND hWnd, POINT ptBegin, POINT ptEnd, int cxScreen, int cyScreen, HDC hdcMem);

// 保存图片
VOID SaveBmp(HDC hdc, HBITMAP hBitmap, LONG lWidth, LONG lHight);

HINSTANCE g_hinstance;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	TCHAR szClassName[] = TEXT("SCREENSHOTS_CLASS");
	TCHAR szAppName[] = TEXT("ScreenShots");
	MSG msg;
	HWND hwnd;
	HMENU hMenu;
	HACCEL hAccel;

	g_hinstance = hInstance;

	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.lpszClassName = szClassName;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EAGLE));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = 0;

	RegisterClassEx(&wndclass);

	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU));
	hwnd = CreateWindowEx(0, szClassName, szAppName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		400, 300, NULL, hMenu, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACC));
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		if (!TranslateAccelerator(hwnd, hAccel, &msg))
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	static HMENU hMenu;
	static int cxScreen, cyScreen;
	static HDC hdcDesk, hdcMem, hdcMemResult;
	static HBITMAP hBitmapMem, hBitmapResult;
	static POINT ptBegin, ptEnd;
	LONG lWidth, lHeight;
	
	switch (uMsg) {
	case WM_CREATE:
		hMenu = ((CREATESTRUCT*)lParam)->hMenu;
		cxScreen = GetSystemMetrics(SM_CXSCREEN);
		cyScreen = GetSystemMetrics(SM_CYSCREEN);
		printf("screen size: %dX%d\n", cxScreen, cyScreen);

		// 内存DC
		hdcDesk = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
		hdcMem = CreateCompatibleDC(hdcDesk);
		hBitmapMem = CreateCompatibleBitmap(hdcDesk, cxScreen, cyScreen);
		SelectObject(hdcMem, hBitmapMem);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if (!g_bCapturing) {
			BitBlt(hdc, 0, 0, ptEnd.x - ptBegin.x, ptEnd.y - ptBegin.y,
				hdcMem, ptBegin.x, ptBegin.y, SRCCOPY);
		}
		EndPaint(hWnd, &ps);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_START:
			printf("WM_COMMAND ID_START\n");
			break;
		case ID_SAVE:
			printf("WM_COMMAND ID_SAVE\n");
			break;
		case ID_EXIT:
			printf("WM_COMMAND ID_SAVE\n");
			SendMessage(hWnd, WM_CLEAR, 0, 0);
			break;
		}

		return 0;

	case WM_LBUTTONDOWN:
		return 0;

	case WM_MOUSEMOVE:
		if (g_bCapturing && g_nLBtnDownCount == 0) {
			SetCursor(LoadCursor(NULL, IDC_CROSS));
			DrawCrossLine(hWnd, ptBegin, cxScreen, cyScreen, hdcMem);


		}
		return 0;

	case WM_DESTROY:
		DeleteObject(hBitmapMem);
		DeleteDC(hdcDesk);
		DeleteDC(hdcMem);
		if (hBitmapResult) {
			DeleteObject(hBitmapResult);
		}
		if (hdcMemResult) {
			DeleteDC(hdcMemResult);
		}

		PostQuitMessage(0);
		return 0;
	}


	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// 第1次鼠标按下，画十字线
VOID DrawCrossLine(HWND hWnd, POINT ptBegin, int cxScreen, int cyScreen, HDC hdcMem) {
	HDC hdc = GetDC(hWnd);
	SelectObject(hdcMem, CreatePen(PS_SOLID, 1, RGB(0, 255, 0)));
	SetROP2(hdcMem, R2_XORPEN);
	MoveToEx(hdcMem, 0, ptBegin.y, NULL);
	LineTo(hdcMem, cxScreen, ptBegin.y);

	MoveToEx(hdcMem, ptBegin.x, 0, NULL);
	LineTo(hdcMem, ptBegin.x, cyScreen);

	BitBlt(hdc, 0, 0, cxScreen, cyScreen, hdcMem, 0, 0, SRCCOPY);
	DeleteObject(SelectObject(hdcMem, GetStockObject(BLACK_PEN)));
	ReleaseDC(hWnd, hdc);
}

// 第2次鼠标按下, 画矩形
VOID DrawRect(HWND hWnd, POINT ptBegin, POINT ptEnd, int cxScreen, int cyScreen, HDC hdcMem) {
	HDC hdc = GetDC(hWnd);
	SelectObject(hdcMem, CreatePen(PS_SOLID, 1, RGB(125, 0, 125)));
	SetROP2(hdcMem, R2_NOTXORPEN);
	Rectangle(hdcMem, ptBegin.x, ptBegin.y, ptEnd.x, ptEnd.y);
	BitBlt(hdc, 0, 0, cxScreen, cyScreen, hdcMem, 0, 0, SRCCOPY);
	DeleteObject(SelectObject(hdcMem, GetStockObject(BLACK_PEN)));
	ReleaseDC(hWnd, hdc);
}

// 保存图片
VOID SaveBmp(HDC hdc, HBITMAP hBitmap, LONG lWidth, LONG lHight) {

}