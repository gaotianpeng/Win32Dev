#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ����Ĭ�ϰ�ť
VOID OnDefPushButton(HWND hwnd);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	TCHAR szClassName[] = TEXT("BUTTON_CLASS");
	TCHAR szAppName[] = TEXT("BUTTON");
	MSG msg;
	HWND hwnd;

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
		300, 600, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

struct {
	int m_nStyle;
	PTSTR m_pText;
}Buttons[] = {
	BS_PUSHBUTTON | BS_NOTIFY | WS_TABSTOP,                 TEXT("��ͨ��ť"), // CtrlID 1000
	BS_ICON | BS_NOTIFY | WS_TABSTOP,                       TEXT("ͼ�갴ť"),
	BS_BITMAP | BS_NOTIFY | WS_TABSTOP,                     TEXT("λͼ��ť"),
	BS_OWNERDRAW,                                           TEXT("�Ի水ť"),

	BS_GROUPBOX,                                            TEXT("������ò"), // CtrlID 1004
	BS_AUTORADIOBUTTON | BS_NOTIFY | WS_GROUP | WS_TABSTOP, TEXT("�й���Ա"),
	BS_AUTORADIOBUTTON | BS_NOTIFY,                         TEXT("������Ա"),
	BS_AUTORADIOBUTTON | BS_NOTIFY,                         TEXT("�޵�����ʿ"),

	BS_GROUPBOX,                                            TEXT("���˰���"), // CtrlID 1008
	BS_AUTOCHECKBOX | BS_NOTIFY | WS_GROUP | WS_TABSTOP,    TEXT("����"),
	BS_AUTOCHECKBOX | BS_NOTIFY,                            TEXT("����"),
	BS_AUTOCHECKBOX | BS_NOTIFY,                            TEXT("������"),

	BS_GROUPBOX,                                            TEXT("�����ƺ�"), // CtrlID 1012
	BS_AUTO3STATE | BS_NOTIFY | WS_GROUP | WS_TABSTOP,      TEXT("�ŶӺ���"),
	BS_AUTO3STATE | BS_NOTIFY,                              TEXT("��������"),
	BS_AUTO3STATE | BS_NOTIFY,                              TEXT("�Ƚ�����"),

	BS_DEFPUSHBUTTON | BS_NOTIFY | WS_TABSTOP,              TEXT("Ĭ�ϰ�ť"), // CtrlID 1016
};

#define NUM (sizeof(Buttons) / sizeof(Buttons[0]))

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static HWND hwndButton[NUM];	// �Ӵ��ڿؼ��������
	int arrPos[NUM] = { 10, 40, 70, 100,         // ÿ���Ӵ��ڿؼ�����ʼY����
	130, 150, 180, 210,
	250, 270, 300, 330,
	370, 390, 420, 450,
	490 };
	
	LPDRAWITEMSTRUCT lpDIS;

	switch (uMsg) {
	case WM_CREATE:
		for (int i = 0; i < NUM; i++) {
			hwndButton[i] = CreateWindowEx(0, TEXT("Button"), Buttons[i].m_pText,
				WS_CHILD|WS_VISIBLE|Buttons[i].m_nStyle, 20, arrPos[i],
				150, 25, hWnd, (HMENU)(1000 + i),
				((LPCREATESTRUCT)lParam)->hInstance, NULL);
		}

		// �ƶ�3��������λ��
		MoveWindow(hwndButton[4], 10, arrPos[4], 170, 115, TRUE);
		MoveWindow(hwndButton[8], 10, arrPos[8], 170, 115, TRUE);
		MoveWindow(hwndButton[12], 10, arrPos[12], 170, 115, TRUE);

		// Ϊͼ�갴ť��λͼ��ť����ͼ�ꡢλͼ
		SendDlgItemMessage(hWnd, IDC_ICONBUTTON, BM_SETIMAGE, IMAGE_ICON,
			(LPARAM)LoadImage(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDI_SMILE),
					IMAGE_ICON, 20, 20, LR_DEFAULTCOLOR));
		SendDlgItemMessage(hWnd, IDC_BITMAPBUTTON, BM_SETIMAGE, IMAGE_BITMAP,
			(LPARAM)LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_KONGLONG)));

		// ����Ĭ�ϰ�ť���ı�
		SetDlgItemText(hWnd, IDC_DEFPUSHBUTTON, TEXT("��ȡ��ѡ��ѡ״̬"));
		// ��ѡ��ť�顢��ѡ��ť�顢��̬��ѡ��ť�飬Ĭ������·ֱ�ѡ��һ��
		CheckRadioButton(hWnd, IDC_AUTORADIOBUTTON1, IDC_AUTORADIOBUTTON3, IDC_AUTORADIOBUTTON2);
		CheckDlgButton(hWnd, IDC_AUTOCHECKBOX3, BST_CHECKED);
		CheckDlgButton(hWnd, IDC_AUTO3STATE2, BST_INDETERMINATE);
		return 0;
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {
			switch (LOWORD(wParam)) {
			case IDC_PUSHBUTTON:      break;
			case IDC_ICONBUTTON:      break;
			case IDC_BITMAPBUTTON:    break;
			case IDC_OWNERDRAWBUTTON: break;
			case IDC_AUTORADIOBUTTON1:break;
			case IDC_AUTORADIOBUTTON2:break;
			case IDC_AUTORADIOBUTTON3:break;
			case IDC_AUTOCHECKBOX1:   break;
			case IDC_AUTOCHECKBOX2:   break;
			case IDC_AUTOCHECKBOX3:   break;
			case IDC_AUTO3STATE1:     break;
			case IDC_AUTO3STATE2:     break;
			case IDC_AUTO3STATE3:     break;
			case IDC_DEFPUSHBUTTON:   OnDefPushButton(hWnd); break;
			}
		}
		return 0;
	case WM_DRAWITEM:
		lpDIS = (LPDRAWITEMSTRUCT)lParam;
		// �ȰѰ�ť�������Ϊ�ʹ��ڱ���һ���İ�ɫ, Ȼ��һ����ɫԲ�Ǿ���
		SelectObject(lpDIS->hDC, GetStockObject(NULL_PEN));
		SelectObject(lpDIS->hDC, GetStockObject(WHITE_BRUSH));
		Rectangle(lpDIS->hDC, 0, 0, lpDIS->rcItem.right + 1, lpDIS->rcItem.bottom + 1);
		SelectObject(lpDIS->hDC, GetStockObject(BLACK_BRUSH));
		RoundRect(lpDIS->hDC, 0, 0, lpDIS->rcItem.right + 1, lpDIS->rcItem.bottom + 1, 20, 20);

		// ���û������ť��ʱ�򣬻���COLOR_HIGHLIGHT��ɫ��Բ�Ǿ���
		if (lpDIS->itemState & ODS_SELECTED) {
			SelectObject(lpDIS->hDC, GetSysColorBrush(COLOR_HIGHLIGHT));
			RoundRect(lpDIS->hDC, 0, 0, lpDIS->rcItem.right + 1, lpDIS->rcItem.bottom + 1, 20, 20);
		}
		// ����ť��ý����ʱ�򣬿��Ի���һ���������
		if (lpDIS->itemState & ODS_FOCUS)
		{
			InflateRect(&lpDIS->rcItem, -2, -2);
			DrawFocusRect(lpDIS->hDC, &lpDIS->rcItem);
		}
		// �Ի水ť���ı���͸�������İ�ɫ����
		SetBkMode(lpDIS->hDC, TRANSPARENT);
		SetTextColor(lpDIS->hDC, RGB(255, 255, 255));
		DrawText(lpDIS->hDC, TEXT("�Ի水ť"), _tcslen(TEXT("�Ի水ť")),
			&lpDIS->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		// �ָ��豸����
		SelectObject(lpDIS->hDC, GetStockObject(BLACK_PEN));
		SelectObject(lpDIS->hDC, GetStockObject(WHITE_BRUSH));
		return TRUE;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

VOID OnDefPushButton(HWND hwnd)
{
	TCHAR szBuf[128] = { 0 };

	if (IsDlgButtonChecked(hwnd, IDC_AUTORADIOBUTTON1) & BST_CHECKED)
		StringCchCopy(szBuf, _countof(szBuf), TEXT("������ò���й���Ա\n"));
	if (IsDlgButtonChecked(hwnd, IDC_AUTORADIOBUTTON2) & BST_CHECKED)
		StringCchCopy(szBuf, _countof(szBuf), TEXT("������ò��������Ա\n"));
	if (IsDlgButtonChecked(hwnd, IDC_AUTORADIOBUTTON3) & BST_CHECKED)
		StringCchCopy(szBuf, _countof(szBuf), TEXT("������ò���޵�����ʿ\n"));

	StringCchCat(szBuf, _countof(szBuf), TEXT("���˰��ã�"));
	if (IsDlgButtonChecked(hwnd, IDC_AUTOCHECKBOX1) & BST_CHECKED)
		StringCchCat(szBuf, _countof(szBuf), TEXT("���� "));
	if (IsDlgButtonChecked(hwnd, IDC_AUTOCHECKBOX2) & BST_CHECKED)
		StringCchCat(szBuf, _countof(szBuf), TEXT("���� "));
	if (IsDlgButtonChecked(hwnd, IDC_AUTOCHECKBOX3) & BST_CHECKED)
		StringCchCat(szBuf, _countof(szBuf), TEXT("������"));
	StringCchCat(szBuf, _countof(szBuf), TEXT("\n"));

	StringCchCat(szBuf, _countof(szBuf), TEXT("�����ƺţ�"));
	if (IsDlgButtonChecked(hwnd, IDC_AUTO3STATE1) & BST_CHECKED)
		StringCchCat(szBuf, _countof(szBuf), TEXT("�ŶӺ��� "));
	if (IsDlgButtonChecked(hwnd, IDC_AUTO3STATE2) & BST_CHECKED)
		StringCchCat(szBuf, _countof(szBuf), TEXT("�������� "));
	if (IsDlgButtonChecked(hwnd, IDC_AUTO3STATE3) & BST_CHECKED)
		StringCchCat(szBuf, _countof(szBuf), TEXT("�Ƚ�����"));
	StringCchCat(szBuf, _countof(szBuf), TEXT("\n"));

	MessageBox(hwnd, szBuf, TEXT("���˼�����"), MB_OK);
}