#include "application.h"
#include "window.h"
#include <commctrl.h>
#include <uxtheme.h>

App::App() {
	instance_ = ::GetModuleHandle(nullptr);
	Instance(this);
}

App::~App() {
}

BOOL App::InitInstance() {
	return TRUE;
}

BOOL App::InitCommonControls(DWORD flags) const {
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC = flags;

	return ::InitCommonControlsEx(&icc);
}

int App::MessageLoop() {
	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(LOWORD(msg.wParam));
}

void App::PostQuitMessage(int exit_code) {
	::PostQuitMessage(exit_code);
}

int App::Run() {
	if (InitInstance()) {
		return MessageLoop();
	}
	else {
		::PostQuitMessage(-1);
		return -1;
	}
}

App& App::Instance(App* app) {
	static App* app_ = app;
	return *app_;
}