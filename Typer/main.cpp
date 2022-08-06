#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wndclass;
	TCHAR szClassName[] = TEXT("Windows");
	TCHAR szAppName[] = TEXT("Typer");

	HWND hwnd;
	MSG msg;

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
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
	
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
};

#define BUFSIZE 65535

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	TEXTMETRIC tm;

	static int cxChar = 0;		// 字符宽度
	static int cxAverChar;		// 平均字符高度
	static int cyChar;			// 字符高度
	static int cxClient;		// 客户区宽度
	static int cyClient;		// 客户区高度
	static DWORD dwLineLen;		// 一行的长度
	static DWORD dwLines;		// 客户区的文本行数
	static int nCaretPosX = 0;	// 插入符号的水平位置
	static int nCaretPosY = 0;	// 插入符号的垂直位置
	static int nCh = 0;			// 缓冲区中的字符个数
	static int nCurChar = 0;	// 当前字符的索引
	static PTCHAR pChInputBuf;	// 输入缓冲区
	TCHAR ch;					// 当前字符
	RECT rect;

	switch (uMsg) {
	case WM_CREATE: {
		// 获取平均字符宽度和字符高度
		hdc = GetDC(hwnd);
		GetTextMetrics(hdc, &tm);
		cxAverChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight;
		ReleaseDC(hwnd, hdc);
		// 分配缓冲区，以存储键盘输入
		pChInputBuf = new TCHAR[BUFSIZE];
		ZeroMemory(pChInputBuf, BUFSIZE);
		printf("字符平均宽度:%d\n", cxAverChar);
		printf("字符高度: %d\n", cyChar);
		return 0;
	}
	break;
	case WM_SIZE: {
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		printf("客户区宽度  ：%d\n", cxClient);
		printf("客户区高度  ：%d\n", cyClient);
		// 计算行的最大宽度和客户区的最大行数
		dwLineLen = cxClient - cxAverChar;
		dwLines = cyClient / cyChar;
		printf("单行最大字符个数：%d\n", dwLineLen);
		printf("当前窗口最大行数: %d\n", dwLines);
	}
	break;
	case WM_SETFOCUS: {
		// 拥有键盘焦点以后创建、显示插入符号
		CreateCaret(hwnd, (HBITMAP)1, 10, cyChar);
		ShowCaret(hwnd);
		SetCaretPos(nCaretPosX, nCaretPosY * cyChar);
		return 0;
	}
	case WM_KILLFOCUS: {
		HideCaret(hwnd);
		DestroyCaret();
		return 0;
	}
	case WM_CHAR: {
		switch (wParam) {
		case '\b':
		case '\n':
		case 0x1B: // Esc
			return 0;
		case '\t':
			for (int i = 0; i < 4; i++) {
				SendMessage(hwnd, WM_CHAR, ' ', 0);
			}
			return 0;
		case '\r':
			pChInputBuf[nCh++] = '\r';
			nCaretPosX = 0;
			nCaretPosY += 1;
			break;
		default: // 显示字符
			HideCaret(hwnd);
			ch = (TCHAR)wParam;
			// 获取字符宽度并显示
			hdc = GetDC(hwnd);
			GetCharWidth32(hdc, (UINT)ch, (UINT)ch, &cxChar);
			TextOut(hdc, nCaretPosX, nCaretPosY * cyChar, &ch, 1);
			ReleaseDC(hwnd, hdc);
			// 保存字符到输入缓冲区
			pChInputBuf[nCh++] = ch;
			nCaretPosX += cxChar;
			if ((DWORD)nCaretPosX > dwLineLen) {
				nCaretPosX = 0;
				pChInputBuf[nCh++] = '\r';
				nCaretPosY++;
			}
			nCurChar = nCh;
			ShowCaret(hwnd);
			break;
		}
		SetCaretPos(nCaretPosX, nCaretPosY * cyChar);
		return 0;
	}
	case WM_KEYDOWN: {
		switch (wParam) {
			case VK_LEFT: {
				printf("VK_LEFT 移动至行首\n");
				if (nCaretPosX > 0) { // 简单设置为只能移动到当前行首
					HideCaret(hwnd);
					ch = pChInputBuf[--nCurChar];
					hdc = GetDC(hwnd);
					GetCharWidth32(hdc, ch, ch, &cxChar);
					ReleaseDC(hwnd, hdc);
					nCaretPosX = max(nCaretPosX - cxChar, 0);
					ShowCaret(hwnd);
				}
				break;
			}
			case VK_RIGHT:
			{
				printf("VK_RIGHT\n");
				break;
			}

		}
		SetCaretPos(nCaretPosX, nCaretPosY * cyChar);
		return 0;
	}

	case WM_PAINT: {
		hdc = BeginPaint(hwnd, &ps);
		HideCaret(hwnd);
		SetRect(&rect, 0, 0, dwLineLen, cyClient);
		DrawText(hdc, pChInputBuf, -1, &rect, DT_LEFT);
		ShowCaret(hwnd);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_DESTROY: {
		delete[] pChInputBuf;
		printf("WM_DESTROY\n");
		PostQuitMessage(0);
	}
	break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}