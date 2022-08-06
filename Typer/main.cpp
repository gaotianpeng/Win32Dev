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

	static int cxChar = 0;		// �ַ����
	static int cxAverChar;		// ƽ���ַ��߶�
	static int cyChar;			// �ַ��߶�
	static int cxClient;		// �ͻ������
	static int cyClient;		// �ͻ����߶�
	static DWORD dwLineLen;		// һ�еĳ���
	static DWORD dwLines;		// �ͻ������ı�����
	static int nCaretPosX = 0;	// ������ŵ�ˮƽλ��
	static int nCaretPosY = 0;	// ������ŵĴ�ֱλ��
	static int nCh = 0;			// �������е��ַ�����
	static int nCurChar = 0;	// ��ǰ�ַ�������
	static PTCHAR pChInputBuf;	// ���뻺����
	TCHAR ch;					// ��ǰ�ַ�
	RECT rect;

	switch (uMsg) {
	case WM_CREATE: {
		// ��ȡƽ���ַ���Ⱥ��ַ��߶�
		hdc = GetDC(hwnd);
		GetTextMetrics(hdc, &tm);
		cxAverChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight;
		ReleaseDC(hwnd, hdc);
		// ���仺�������Դ洢��������
		pChInputBuf = new TCHAR[BUFSIZE];
		ZeroMemory(pChInputBuf, BUFSIZE);
		printf("�ַ�ƽ�����:%d\n", cxAverChar);
		printf("�ַ��߶�: %d\n", cyChar);
		return 0;
	}
	break;
	case WM_SIZE: {
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		printf("�ͻ������  ��%d\n", cxClient);
		printf("�ͻ����߶�  ��%d\n", cyClient);
		// �����е�����ȺͿͻ������������
		dwLineLen = cxClient - cxAverChar;
		dwLines = cyClient / cyChar;
		printf("��������ַ�������%d\n", dwLineLen);
		printf("��ǰ�����������: %d\n", dwLines);
	}
	break;
	case WM_SETFOCUS: {
		// ӵ�м��̽����Ժ󴴽�����ʾ�������
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
		default: // ��ʾ�ַ�
			HideCaret(hwnd);
			ch = (TCHAR)wParam;
			// ��ȡ�ַ���Ȳ���ʾ
			hdc = GetDC(hwnd);
			GetCharWidth32(hdc, (UINT)ch, (UINT)ch, &cxChar);
			TextOut(hdc, nCaretPosX, nCaretPosY * cyChar, &ch, 1);
			ReleaseDC(hwnd, hdc);
			// �����ַ������뻺����
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
				printf("VK_LEFT �ƶ�������\n");
				if (nCaretPosX > 0) { // ������Ϊֻ���ƶ�����ǰ����
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