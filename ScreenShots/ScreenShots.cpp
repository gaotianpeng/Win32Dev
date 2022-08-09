#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include "resource.h"

BOOL g_bCapturing = FALSE;
INT g_nLBtnDownCount = 0;

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
			hdc = GetDC(hWnd);
			// 先将本程序最小化, 截取整个屏幕内容到hdcMem
			SetMenu(hWnd, NULL);
			ShowWindow(hWnd, SW_MINIMIZE);
			Sleep(500);
			BitBlt(hdcMem, 0, 0, cxScreen, cyScreen, hdcDesk, 0, 0, SRCCOPY);

			// 去掉程序的标题栏、边框等, 然后最大化显示, 客户区显示的是整个屏幕的内容
			SetWindowLongPtr(hWnd, GWL_STYLE, (LONG)(WS_OVERLAPPED | WS_VISIBLE));
			ShowWindow(hWnd, SW_RESTORE);
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, cxScreen, cyScreen, SWP_SHOWWINDOW);
			BitBlt(hdc, 0, 0, cxScreen, cyScreen, hdcMem, 0, 0, SRCCOPY);
			ReleaseDC(hWnd, hdc);

			SetCursor(LoadCursor(NULL, IDC_CROSS));
			ptBegin = ptEnd = { 0, 0 };
			DrawCrossLine(hWnd, ptBegin, cxScreen, cyScreen, hdcMem);
			g_bCapturing = TRUE;
			break;
		case ID_SAVE:
			printf("WM_COMMAND ID_SAVE\n");
			lWidth = ptEnd.x - ptBegin.x + 1;
			lHeight = ptEnd.y - ptBegin.y + 1;
			hdc = GetDC(hWnd);
			hdcMemResult = CreateCompatibleDC(hdc);
			hBitmapResult = CreateCompatibleBitmap(hdc, lWidth, lHeight);
			SelectObject(hdcMemResult, hBitmapResult);
			BitBlt(hdcMemResult, 0, 0, lWidth, lHeight, hdcMem, ptBegin.x, ptBegin.y, SRCCOPY);
			SaveBmp(hdc, hBitmapResult, lWidth, lHeight);
			ReleaseDC(hWnd, hdc);
			break;
		case ID_EXIT:
			printf("WM_COMMAND ID_SAVE\n");
			SendMessage(hWnd, WM_CLEAR, 0, 0);
			break;
		}

		return 0;

	case WM_LBUTTONDOWN:
		// 第一次按下鼠标左键
		if (g_bCapturing && g_nLBtnDownCount == 0) {
			SetCursor(LoadCursor(NULL, IDC_CROSS));
			// 擦除WM_MOUSEMOVE消息中最后一次画的十字线
			DrawCrossLine(hWnd, ptBegin, cxScreen, cyScreen, hdcMem);
			ptEnd = ptBegin;
			g_nLBtnDownCount = 1;
		}
		// 第二次按下鼠标左键
		else if (g_bCapturing && g_nLBtnDownCount == 1) {
			g_bCapturing = FALSE;
			g_nLBtnDownCount = 0;

			DrawRect(hWnd, ptBegin, ptEnd, cxScreen, cyScreen, hdcMem);
			SetWindowLongPtr(hWnd, GWL_STYLE, (LONG)(WS_OVERLAPPEDWINDOW | WS_VISIBLE));
			SetMenu(hWnd, hMenu);
			SetWindowPos(hWnd, HWND_NOTOPMOST, 200, 100, 400, 300, SWP_SHOWWINDOW);
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			// 结束点的坐标可能小于起始点
			if (ptEnd.x < ptBegin.x) {
				int x = ptBegin.x;
				ptBegin.x = ptEnd.x;
				ptEnd.x = x;
			}
			
			if (ptEnd.y < ptBegin.y) {
				int y = ptBegin.y;
				ptBegin.y = ptEnd.y;
				ptEnd.y = y;
			}
		}
		return 0;

	case WM_MOUSEMOVE:
		if (g_bCapturing && g_nLBtnDownCount == 0) {
			SetCursor(LoadCursor(NULL, IDC_CROSS));
			// 擦除上一条十字线
			DrawCrossLine(hWnd, ptBegin, cxScreen, cyScreen, hdcMem);

			ptBegin.x = LOWORD(lParam);
			ptBegin.y = HIWORD(lParam);
			DrawCrossLine(hWnd, ptBegin, cxScreen, cyScreen, hdcMem);
		}
		else if (g_bCapturing && g_nLBtnDownCount == 1) {
			SetCursor(LoadCursor(NULL, IDC_CROSS));
			// 擦除上一个矩形
			DrawRect(hWnd, ptBegin, ptEnd, cxScreen, cyScreen, hdcMem);
			
			ptEnd.x = LOWORD(lParam);
			ptEnd.y = HIWORD(lParam);
			DrawRect(hWnd, ptBegin, ptEnd, cxScreen, cyScreen, hdcMem);
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
VOID SaveBmp(HDC hdc, HBITMAP hBitmap, LONG lWidth, LONG lHeight) {
	BITMAPFILEHEADER bmfh = { 0 };
	BITMAPINFOHEADER bmih = { 0 };
	HANDLE hFile;
	DWORD dwPixelSize;
	LPVOID lpBmpData;

	dwPixelSize = lWidth * lHeight * 4; // 32位位图
	lpBmpData = new BYTE[dwPixelSize];
	DWORD dwBytesWritten;
	SYSTEMTIME stLocal;
	TCHAR szFileName[32] = { 0 };

	GetLocalTime(&stLocal);
	wsprintf(szFileName, TEXT("%d%0.2d%0.2d-%0.2d%0.2d%0.2d"),
		stLocal.wYear, stLocal.wMonth, stLocal.wDay,
		stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
	StringCchCat(szFileName, _countof(szFileName), TEXT(".bmp"));

	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = lWidth;
	bmih.biHeight = lHeight;
	bmih.biPlanes = 1;
	bmih.biBitCount = 32;
	bmih.biCompression = BI_RGB;

	bmfh.bfType = 0x4D42;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfh.bfSize = dwPixelSize + bmfh.bfOffBits;

	GetDIBits(hdc, hBitmap, 0, lHeight, lpBmpData, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS);
	hFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hFile, &bmfh, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, &bmih, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, lpBmpData, dwPixelSize, &dwBytesWritten, NULL);
	delete[] lpBmpData;
}