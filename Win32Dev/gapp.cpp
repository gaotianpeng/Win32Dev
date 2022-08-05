#include "gapp.h"
#include "config.h"
#include "window.h"
#include <windows.h>
#include <commctrl.h>

static void ActivateWindow(HWND hwnd) {
	if (::IsIconic(hwnd)) {
		::ShowWindow(hwnd, SW_RESTORE);
	}

	if (!IsWindowVisible(hwnd)) {
		WINDOWPLACEMENT wp = { 0 };
		wp.length = sizeof(WINDOWPLACEMENT);
		::GetWindowPlacement(hwnd, &wp);
		::ShowWindow(hwnd, wp.showCmd);
	}

	::SetForegroundWindow(hwnd);
	::BringWindowToTop(hwnd);
}

static bool CheckInstance(LPCWSTR mutex_name, LPCWSTR class_name) {
	if (::CreateMutexW(NULL, FALSE, mutex_name) == NULL ||
			GetLastError() == ERROR_ALREADY_EXISTS || 
			GetLastError() == ERROR_ACCESS_DENIED) {
		HWND hwnd = FindWindow(class_name, NULL);
		if (::IsWindow(hwnd)) {
			ActivateWindow(hwnd);
			::FlashWindow(hwnd, TRUE);
		}

		return true;
	} 


	return false;
}

static Window win;

GApp::~GApp() {
}

BOOL GApp::InitInstance() {
	if (CheckInstance(APP_MUTEX, L"Win32Dev")) {
		MessageBox(NULL, TEXT("Already running"), TEXT("ERROR"), MB_OK);
		return false;
	}

	win.Create(NULL);
	InitCommonControls(ICC_STANDARD_CLASSES);

	return true;
}

void GApp::Uninitialize() {
	PostQuitMessage();
}