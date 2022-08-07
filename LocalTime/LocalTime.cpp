#include <Windows.h>
#include <tchar.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	TCHAR szAppName[] = TEXT("LocalTime");
	TCHAR szClassName[] = TEXT("LocalTimeClass");
	HWND hWnd;
	MSG msg;

	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpszClassName = szClassName;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = NULL;

	RegisterClassEx(&wndclass);
	hWnd = CreateWindowEx(0, szClassName, szAppName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	SIZE size;
	SYSTEMTIME stLocal;
	LPTSTR arrWeek[] = { TEXT("星期日"), TEXT("星期一"), TEXT("星期二"), TEXT("星期三"),
		TEXT("星期四"), TEXT("星期五"), TEXT("星期六") };
	TCHAR szBuf[32] = { 0 };

	switch (uMsg) {
	case WM_CREATE:
		hdc = GetDC(hWnd);
		GetLocalTime(&stLocal);
		wsprintf(szBuf, TEXT("%d年%02d月%02d日 %s %02d:%02d:%02d"), stLocal.wYear,
			stLocal.wMonth, stLocal.wDay, arrWeek[stLocal.wDayOfWeek],
			stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
		GetTextExtentPoint32(hdc, szBuf, _tcslen(szBuf), &size);
		SetRect(&rect, 0, 0, size.cx, size.cy);
		AdjustWindowRectEx(&rect, GetWindowLongPtr(hWnd, GWL_STYLE),
			GetMenu(hWnd) != NULL, GetWindowLongPtr(hWnd, GWL_EXSTYLE));
		SetWindowPos(hWnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
			SWP_NOZORDER | SWP_NOMOVE);
		ReleaseDC(hWnd, hdc);
		// 创建计时器
		SetTimer(hWnd, 1, 1000, NULL);
		return 0;
	case WM_TIMER:
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetLocalTime(&stLocal);
		wsprintf(szBuf, TEXT("%d年%02d月%02d日 %s %02d:%02d:%02d"),
			stLocal.wYear, stLocal.wMonth, stLocal.wDay, arrWeek[stLocal.wDayOfWeek],
			stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
		TextOut(hdc, 0, 0, szBuf, _tcslen(szBuf));
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
