#include <Windows.h>
#include <Windowsx.h>
#include <stdio.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID DrawFrame(HWND hwnd, POINT ptLeftTop, POINT ptRightTop);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wndclass;
	TCHAR szClassName[] = TEXT("DrawClass");
	TCHAR szAppName[] = TEXT("DrawApp");
	HWND hwnd;
	MSG msg;

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szClassName;
	wndclass.hIconSm = NULL;
	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(0, szClassName, szAppName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, 400, 300, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, hwnd, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	static POINT ptLeftTop, ptRightBottom;
	static BOOL bStarting;

	switch (uMsg) {
	case WM_LBUTTONDOWN:
		printf("WM_LBUTTON %d, %d\n", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		SetCursor(LoadCursor(NULL, IDC_CROSS));

		// ��ʼ���¾��ε����ϽǺ����½�����
		ptLeftTop.x = ptRightBottom.x = GET_X_LPARAM(lParam);
		ptLeftTop.y = ptRightBottom.y = GET_Y_LPARAM(lParam);
		bStarting = TRUE;
		break;
	case WM_MOUSEMOVE:
		if (bStarting) {
			SetCursor(LoadCursor(NULL, IDC_CROSS));
			// �Ȳ�����һ�� WM_MOUSEMOVE ��Ϣ�������Σ� WM_MOUSEMOVE ��Ϣ�᲻�ϲ���
			DrawFrame(hWnd, ptLeftTop, ptRightBottom);

			// �¾��ε����½�����
			ptRightBottom.x = GET_X_LPARAM(lParam);
			ptRightBottom.y = GET_Y_LPARAM(lParam);
			// �����¾���
			DrawFrame(hWnd, ptLeftTop, ptRightBottom);
		}
		return 0;
	case WM_LBUTTONUP:
		printf("WM_LBUTTON %d, %d\n", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		if (bStarting) {
			// ����WM_MOUSEMOVE��Ϣ�����������һ������
			DrawFrame(hWnd, ptLeftTop, ptRightBottom);

			// ���վ��ε����½�����
			ptRightBottom.x = GET_X_LPARAM(lParam);
			ptRightBottom.y = GET_Y_LPARAM(lParam);

			SetCursor(LoadCursor(NULL, IDC_ARROW));
			bStarting = FALSE;

			// �������յľ���
			InvalidateRect(hWnd, NULL, TRUE);
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		Rectangle(hdc, ptLeftTop.x, ptLeftTop.y, ptRightBottom.x, ptRightBottom.y);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}


	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

VOID DrawFrame(HWND hwnd, POINT ptLeftTop, POINT ptRightBottom) {
	HDC hdc = GetDC(hwnd);
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	SetROP2(hdc, R2_NOT);
	Rectangle(hdc, ptLeftTop.x, ptLeftTop.y, ptRightBottom.x, ptRightBottom.y);
	SelectObject(hdc, GetStockObject(WHITE_BRUSH));
	ReleaseDC(hwnd, hdc);
}