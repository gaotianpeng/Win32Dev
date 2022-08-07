#include <Windows.h>
#include <stdio.h>
#include <tchar.h>                  // _tcslen������Ҫ��ͷ�ļ�
#include "HelloDLL.h"
#include "resource.h"

#pragma comment(lib, "Winmm.lib")   // ����������PlaySound������ҪWinmm�����
#pragma comment(lib, "HelloDLL.lib")


// �������������ڹ���
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wndclass;                        // RegisterClassEx�����õ�WNDCLASSEX�ṹ
    TCHAR szClassName[] = TEXT("MyWindow");     // RegisterClassEx����ע��Ĵ����������
    TCHAR szAppName[] = TEXT("HelloWindows");   // ���ڱ���
    HWND hwnd;                                  // CreateWindowEx���������Ĵ��ڵľ��
    MSG msg;                                    // ��Ϣѭ�����õ���Ϣ�ṹ��

    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WindowProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    //wndclass.lpszMenuName = NULL;
    wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);  // ���ز˵���Դ ����1
    wndclass.lpszClassName = szClassName;
    wndclass.hIconSm = NULL;
    RegisterClassEx(&wndclass);

    // ���ز˵���Դ ����2
    //HMENU hMenu;
    //hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
    //hwnd = CreateWindowEx(0, szClassName, szAppName, WS_OVERLAPPEDWINDOW,
    //    CW_USEDEFAULT, CW_USEDEFAULT, 300, 180, NULL, hMenu, hInstance, NULL);

    hwnd = CreateWindowEx(0, szClassName, szAppName, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        if (!TranslateAccelerator(hwnd, hAccel, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
        }
    }

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    PAINTSTRUCT ps1;
    TCHAR szStr[] = TEXT("��ã�Windows�������");
    HMENU hMenu;
    TCHAR szBuf[64] = { 0 };

    switch (uMsg) {
    case WM_CREATE: {
		PlaySound(TEXT("�ɶ�(�����).wav"), NULL, SND_FILENAME | SND_ASYNC/* | SND_LOOP*/);
		printf("%d\n", funcAdd(1, 2));
        nValue = 10;
        ps.x = 10;
        ps.y = 10;
        printf("%d %d %d\n", nValue, ps.x, ps.y);

        // ���ز˵���Դ ����3
        //hMenu = LoadMenu(g_hInstace, MAKEINTRESOURCE(IDR_MENU1));
        //SetMenu(hwnd, hMenu);

		return 0;
    }
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps1);
        TextOut(hdc, 10, 10, szStr, _tcslen(szStr));
        EndPaint(hwnd, &ps1);
        return 0;
    case WM_SIZE:
        InvalidateRect(hwnd, NULL, true);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_ACTIVATE:
        printf("window is activated \n");
        return 0;
    case WM_KEYDOWN:
        return 0;

	case WM_COMMAND:
		printf("WM_COMMAND %d\n", LOWORD(wParam));
		switch (LOWORD(wParam)) {
		case ID_FILE_NEW:
			wsprintf(szBuf, TEXT("�������� �½� �˵��� ����ID %d\n"), ID_FILE_NEW);
			MessageBox(hwnd, szBuf, TEXT("��ʾ"), MB_OK);
			break;
		case ID_EDIT_CUT:
			wsprintf(szBuf, TEXT("�������� ǰ�� �˵��� ����ID %d\n"), ID_FILE_NEW);
			MessageBox(hwnd, szBuf, TEXT("��ʾ"), MB_OK);
			break;
		}

		return 0;
	case WM_SYSCOMMAND:
		printf("WM_SYSCOMMAND %d\n", LOWORD(wParam));
		switch (LOWORD(wParam) & 0xFFF0) {
		case SC_CLOSE:
			wsprintf(szBuf, TEXT("�������� ϵͳ�˵� �ر� �˵��� \n"));
			MessageBox(hwnd, szBuf, TEXT("��ʾ"), MB_OK);
            SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}