#include <Windows.h>
#include <math.h>

const int CLOCK_SIZE = 200; // 钟表的直径
const DOUBLE TWOPI = 2 * 3.1415926;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void DrawDots(HDC hdc);
void DrawLine(HDC hdc, int nAngle, int nRadiusAdjust);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	TCHAR szAppName[] = TEXT("ClockApp");
	TCHAR szClassName[] = TEXT("ClockClass");
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
	hWnd = CreateWindowEx(0, szClassName, szAppName, WS_POPUP|WS_SYSMENU,
		200, 100, CLOCK_SIZE, CLOCK_SIZE, NULL, NULL, hInstance, NULL);

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
	HRGN hRgn;
	SYSTEMTIME st;

	switch (uMsg) {
	case WM_CREATE:
		// 创建椭圆裁剪区域
		hRgn = CreateEllipticRgn(0, 0, CLOCK_SIZE, CLOCK_SIZE);
		SetWindowRgn(hWnd, hRgn, TRUE);
		// 设置为总在最前
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		// 创建计时器
		SetTimer(hWnd, 1, 1000, NULL);
		return 0;
	case WM_TIMER:
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	case WM_LBUTTONDOWN:
		// 按住鼠标左键可以拖动窗口
		SetCursor(LoadCursor(NULL, IDC_HAND));
		SendMessage(hWnd, WM_NCLBUTTONDBLCLK, HTCAPTION, 0);
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetLocalTime(&st);
		// 画点
		SelectObject(hdc, GetStockObject(BLACK_BRUSH));
		DrawDots(hdc);

		// 画秒针、分针、时针
		SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)));
		DrawLine(hdc, st.wSecond * 6, 10); // 秒针、秒针度数 = 秒 * 6

		DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(0, 0, 0))));
		DrawLine(hdc, st.wMinute * 6, 20); // 分针、分针度数 = 分 * 6

		DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 3, RGB(0, 0, 0))));
		DrawLine(hdc, (st.wHour % 12) * 30 + st.wMinute / 2, 30); // 时针, 时针度数 = 时 * 30 + 分/2
		DeleteObject(SelectObject(hdc, GetStockObject(BLACK_PEN)));
		
		SelectObject(hdc, GetStockObject(WHITE_BRUSH));
		EndPaint(hWnd, &ps);

		return 0;
	case WM_CHAR:
		switch (wParam) {
		case '1': // 更换窗口背景
			SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND,
				(LONG)CreatePatternBrush((HBITMAP)LoadImage(NULL, TEXT("Back1.bmp"),
					IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE)));
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case '2': // 更换窗口背景
			SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND,
				(LONG)CreatePatternBrush((HBITMAP)LoadImage(NULL, TEXT("Back2.bmp"),
					IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE)));
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case 0x1B: // Esc
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		return 0;

	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

VOID DrawDots(HDC hdc) {
	int x, y;
	int nRadius;
	for (int nAngle = 0; nAngle < 360; nAngle += 6) {
		x = CLOCK_SIZE / 2 + (int)((CLOCK_SIZE / 2 - 4) * sin(TWOPI * nAngle / 360));
		y = CLOCK_SIZE / 2 - (int)((CLOCK_SIZE / 2 - 4) * cos(TWOPI * nAngle / 360));
		// 每隔30度画大圆，小时的圆点，每隔6度画小圆，分钟的圆点
		nRadius = nAngle % 5 ? 2 : 4;
		Ellipse(hdc, x - nRadius, y - nRadius, x + nRadius, y + nRadius);
	}
}

VOID DrawLine(HDC hdc, int nAngle, int nRadiusAdjust) {
	int x1, y1, x2, y2;
	x1 = CLOCK_SIZE / 2 + (int)(((CLOCK_SIZE / 2 - 4) - nRadiusAdjust) * sin(TWOPI * nAngle / 360));
	y1 = CLOCK_SIZE / 2 - (int)(((CLOCK_SIZE / 2 - 4) - nRadiusAdjust) * cos(TWOPI * nAngle / 360));
	x2 = CLOCK_SIZE / 2 + (int)(10 * sin(TWOPI * ((DOUBLE)nAngle + 180) / 360));
	y2 = CLOCK_SIZE / 2 - (int)(10 * cos(TWOPI * ((DOUBLE)nAngle + 180) / 360));

	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}
