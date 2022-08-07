#include <Windows.h>
#include <stdio.h>
#include <tchar.h>                  // _tcslen函数需要该头文件
#include "HelloDLL.h"
#include "resource.h"

#pragma comment(lib, "Winmm.lib")   // 播放声音的PlaySound函数需要Winmm导入库
#pragma comment(lib, "HelloDLL.lib")


// 函数声明，窗口过程
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wndclass;                        // RegisterClassEx函数用的WNDCLASSEX结构
    TCHAR szClassName[] = TEXT("MyWindow");     // RegisterClassEx函数注册的窗口类的名称
    TCHAR szAppName[] = TEXT("HelloWindows");   // 窗口标题
    HWND hwnd;                                  // CreateWindowEx函数创建的窗口的句柄
    MSG msg;                                    // 消息循环所用的消息结构体

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
    wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);  // 加载菜单资源 方法1
    wndclass.lpszClassName = szClassName;
    wndclass.hIconSm = NULL;
    RegisterClassEx(&wndclass);

    // 加载菜单资源 方法2
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
    TCHAR szStr[] = TEXT("你好，Windows程序设计");
    HMENU hMenu;
    TCHAR szBuf[64] = { 0 };

    switch (uMsg) {
    case WM_CREATE: {
		PlaySound(TEXT("成都(两会版).wav"), NULL, SND_FILENAME | SND_ASYNC/* | SND_LOOP*/);
		printf("%d\n", funcAdd(1, 2));
        nValue = 10;
        ps.x = 10;
        ps.y = 10;
        printf("%d %d %d\n", nValue, ps.x, ps.y);

        // 加载菜单资源 方法3
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
			wsprintf(szBuf, TEXT("您单击了 新建 菜单项 命令ID %d\n"), ID_FILE_NEW);
			MessageBox(hwnd, szBuf, TEXT("提示"), MB_OK);
			break;
		case ID_EDIT_CUT:
			wsprintf(szBuf, TEXT("您单击了 前切 菜单项 命令ID %d\n"), ID_FILE_NEW);
			MessageBox(hwnd, szBuf, TEXT("提示"), MB_OK);
			break;
		}

		return 0;
	case WM_SYSCOMMAND:
		printf("WM_SYSCOMMAND %d\n", LOWORD(wParam));
		switch (LOWORD(wParam) & 0xFFF0) {
		case SC_CLOSE:
			wsprintf(szBuf, TEXT("您单击了 系统菜单 关闭 菜单项 \n"));
			MessageBox(hwnd, szBuf, TEXT("提示"), MB_OK);
            SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}