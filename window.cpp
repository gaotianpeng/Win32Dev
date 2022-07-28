#include "window.h"
#include "window_map.h"
#include <tchar.h>
#include <xstring>
#include <windowsx.h>
#include <windows.h>
#include <iostream>

using namespace std;

Window* Window::current_window_ = nullptr;
const std::wstring default_wnd_class_name_ = L"DefaultWindow";

Window::Window():instance_(::GetModuleHandle(nullptr)),
		h_parent_wnd_(nullptr), h_wnd_(nullptr) {
	current_window_ = nullptr;

	::ZeroMemory(&wnd_class_, sizeof(WNDCLASSEX));
	if (!::GetClassInfoEx(instance_, default_wnd_class_name_.c_str(), &wnd_class_)) {
		wnd_class_.cbSize = sizeof(wnd_class_);
		wnd_class_.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;;
		wnd_class_.lpfnWndProc = WndProcStatic;
		wnd_class_.hInstance = instance_;
		wnd_class_.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
		wnd_class_.hbrBackground = reinterpret_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
		wnd_class_.lpszClassName = default_wnd_class_name_.c_str();
		::RegisterClassEx(&wnd_class_);
	}
}

Window::Window(HWND hwnd): instance_(::GetModuleHandle(nullptr)),
		h_parent_wnd_(nullptr), h_wnd_(nullptr) {
	current_window_ = nullptr;
	h_wnd_ = hwnd;
}

Window::~Window() {
}

HWND Window::Create(HWND parent) {
	PreRegisterClass(wnd_class_);
	if (wnd_class_.lpszClassName) {
		RegisterClass(wnd_class_);
		create_struct_.lpszClass = wnd_class_.lpszClassName;
	}

	PreCreate(create_struct_);
	if (!create_struct_.lpszClass) {
		create_struct_.lpszClass = default_wnd_class_name_.c_str();
	}

	if (!parent && create_struct_.hwndParent) {
		parent = create_struct_.hwndParent;
	}

	DWORD style;
	if (create_struct_.style) {
		style = create_struct_.style;
	}
	else {
		style = WS_VISIBLE | (parent ? WS_CHILD : WS_OVERLAPPEDWINDOW);
	}

	bool cx_or_cy = create_struct_.cx || create_struct_.cy;
	int x = cx_or_cy ? create_struct_.x : CW_USEDEFAULT;
	int y = cx_or_cy ? create_struct_.y : CW_USEDEFAULT;
	int cx = cx_or_cy ? create_struct_.cx : CW_USEDEFAULT;
	int cy = cx_or_cy ? create_struct_.cy : CW_USEDEFAULT;

	return Create(create_struct_.dwExStyle,
		create_struct_.lpszClass,
		create_struct_.lpszName,
		style,
		x, y, cx, cy,
		parent,
		create_struct_.hMenu,
		create_struct_.lpCreateParams);
}

HWND Window::Create(DWORD ex_style, LPCWSTR class_name, LPCWSTR window_name,
		DWORD style, int x, int y, int width, int height, HWND parent,
		HMENU menu, LPVOID param) {
	Destroy();
	current_window_ = this;
	h_parent_wnd_ = parent;
	h_wnd_ = ::CreateWindowEx(ex_style, class_name, window_name, style, x, y,
		width, height, parent, menu, instance_, param);

	WNDCLASSEX wc = { 0 };
	::GetClassInfoEx(instance_, class_name, &wc);
	if (wc.lpfnWndProc != reinterpret_cast<WNDPROC>(WndProcStatic)) {
		OnCreate(h_wnd_, &create_struct_);
	}

	current_window_ = nullptr;

	return h_wnd_;
}

void Window::PreCreate(CREATESTRUCT& cs) {
	create_struct_.cx = cs.cx;
	create_struct_.cy = cs.cy;
	create_struct_.dwExStyle = cs.dwExStyle;
	create_struct_.hInstance = instance_;
	create_struct_.hMenu = cs.hMenu;
	create_struct_.hwndParent = cs.hwndParent;
	create_struct_.lpCreateParams = cs.lpCreateParams;
	create_struct_.lpszClass = cs.lpszClass;
	create_struct_.lpszName = cs.lpszName;
	create_struct_.style = cs.style;
	create_struct_.x = cs.x;
	create_struct_.y = cs.y;
}

void Window::PreRegisterClass(WNDCLASSEX& wc) {
	wnd_class_.style = wc.style;
	wnd_class_.lpfnWndProc = WndProcStatic;
	wnd_class_.cbClsExtra = wc.cbClsExtra;
	wnd_class_.cbWndExtra = wc.cbWndExtra;
	wnd_class_.hInstance = instance_;
	wnd_class_.hIcon = wc.hIcon;
	wnd_class_.hCursor = wc.hCursor;
	wnd_class_.hbrBackground = wc.hbrBackground;
	wnd_class_.lpszMenuName = wc.lpszMenuName;
	wnd_class_.lpszClassName = wc.lpszClassName;
}

BOOL Window::PreTranslateMessage(MSG* msg) {
	return TRUE;
}

BOOL Window::RegisterClass(WNDCLASSEX& wc) const {
	WNDCLASSEX wc_existing = { 0 };
	if (::GetClassInfoEx(instance_, wc.lpszClassName, &wc_existing)) {
		wc = wc_existing;
		return TRUE;
	}

	wc.cbSize = sizeof(wc);
	wc.hInstance = instance_;
	wc.lpfnWndProc = WndProcStatic;

	return ::RegisterClassEx(&wc);
}

void Window::Destroy() {
	WindowMap::Instance().Remove(this);
	h_wnd_ = nullptr;
}


HWND Window::GetWindowHandle() const {
	return h_wnd_;
}

void Window::SetWindowHandle(HWND hwnd) {
	h_wnd_ = hwnd;
}

BOOL Window::BringWindowToTop() const {
	return ::BringWindowToTop(h_wnd_);
}

BOOL Window::OnCommand(WPARAM wparam, LPARAM LPARAM) {
	return true;
}

void Window::OnContextMenu(HWND hwnd, POINT pt) {
}

void Window::OnCreate(HWND hwnd, LPCREATESTRUCT create_struct) {
	LOGFONT logfont;
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(logfont), &logfont);
	font_ = ::CreateFontIndirect(&logfont);
	::SendMessage(h_wnd_, WM_SETFONT, reinterpret_cast<WPARAM>(font_), FALSE);

	cout << "Window::OnCreate ------------" << endl;
}

BOOL Window::OnDestroy() {
	PostQuitMessage(0);
	return TRUE;
}

void Window::OnDropFiles(HDROP drop_info) {
	cout << "Window::OnDropFiles ------------" << endl;
}

void Window::OnGetMinMaxInfo(LPMINMAXINFO mmi) {
}

LRESULT Window::OnMouseEvent(UINT msg, WPARAM wpara, LPARAM lparam) {
	return -1;
}

void Window::OnMove(LPPOINTS pts) {
}

LRESULT Window::OnNotify(int control_id, LPNMHDR nmh) {
	return -1;
}

void Window::OnPaint(HDC hdc, LPPAINTSTRUCT ps) {
}

void Window::OnSize(UINT msg, UINT type, SIZE size) {
}

void Window::OnTaskbarCallback(UINT msg, LPARAM lparam) {
}

void Window::OnTimer(UINT_PTR event_id) {
}

void Window::OnWindowPosChanging(LPWINDOWPOS wnd_pos) {
}

LRESULT Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	return DefaultWndProc(hwnd, msg, wparam, lparam);
}

LRESULT Window::DefaultWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
		case WM_COMMAND: {
			if (OnCommand(wparam, lparam)) {
				return 0;
			}
			break;
		}
		case WM_CONTEXTMENU: {
			POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
			OnContextMenu(reinterpret_cast<HWND>(wparam), pt);
			break;
		}
		case WM_CREATE:
			OnCreate(hwnd, reinterpret_cast<LPCREATESTRUCT>(lparam));
			break;
		case WM_DESTROY: {
			if (OnDestroy()) {
				return 0;
			}
			break;
		}
		case WM_DROPFILES:
			OnDropFiles(reinterpret_cast<HDROP>(wparam));
			break;
		case WM_ENTERSIZEMOVE:
		case WM_EXITSIZEMOVE: {
			SIZE size = { 0 };
			OnSize(msg, 0, size);
			break;
		}
		case WM_GETMINMAXINFO: {
			OnGetMinMaxInfo(reinterpret_cast<LPMINMAXINFO>(lparam));
			break;
		}
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEACTIVATE:
		case WM_MOUSEHOVER:
		case WM_MOUSEHWHEEL:
		case WM_MOUSELEAVE:
		case WM_MOUSEMOVE:
		case WM_MOUSEWHEEL: {
			LRESULT result = OnMouseEvent(msg, wparam, lparam);
			if (result != -1)
				return result;
			break;
		}
		case WM_MOVE: {
			POINTS pts = MAKEPOINTS(lparam);
			OnMove(&pts);
			break;
		}
		case WM_NOTIFY: {
			LRESULT result = OnNotify(static_cast<int>(wparam),
				reinterpret_cast<LPNMHDR>(lparam));
			if (result) {
				return result;
			}
			break;
		}
		case WM_PAINT: {
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);
			TCHAR str[] = TEXT("Hello, Win32");
			TextOut(hdc, 10, 10, str, _tcslen(str));
			EndPaint(hwnd, &ps);
			if (!prev_wnd_proc_) {
				if (::GetUpdateRect(hwnd, nullptr, FALSE)) {
					PAINTSTRUCT ps;
					HDC hdc = ::BeginPaint(hwnd, &ps);
					OnPaint(hdc, &ps);
					::EndPaint(hwnd, &ps);
				}
				else {
					HDC hdc = ::GetDC(hwnd);
					OnPaint(hdc, nullptr);
					::ReleaseDC(hwnd, hdc);
				}
			}
			break;
		}
		case WM_SIZE: {
			SIZE size = { LOWORD(lparam), HIWORD(lparam) };
			OnSize(msg, static_cast<UINT>(wparam), size);
			break;
		}
		case WM_TIMER:
			OnTimer(static_cast<UINT>(wparam));
			break;
		case WM_WINDOWPOSCHANGING:
			OnWindowPosChanging(reinterpret_cast<LPWINDOWPOS>(lparam));
			break;
		default: {
			break;
		}
	}

	if (prev_wnd_proc_) {
		return ::CallWindowProc(prev_wnd_proc_, hwnd, msg, wparam, lparam);
	}
	else {
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}

LRESULT CALLBACK Window::WndProcStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	Window* window = WindowMap::Instance().GetWindow(hwnd);
	if (!window) {
		window = current_window_;
		if (window) {
			window->SetWindowHandle(hwnd);
			WindowMap::Instance().Add(hwnd, window);
		}
	}

	if (window) {
		return window->WndProc(hwnd, msg, wparam, lparam);
	}
	else {
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}

void Window::Subclass(HWND hwnd) {
	WNDPROC current_proc = reinterpret_cast<WNDPROC>(::GetWindowLongPtr(hwnd, GWLP_WNDPROC));
	if (current_proc != reinterpret_cast<WNDPROC>(WndProcStatic)) {
		prev_wnd_proc_ = reinterpret_cast<WNDPROC>(::SetWindowLongPtr(
			hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcStatic)));
		h_wnd_ = hwnd;
	}
}

void Window::UnSubclass() {
	WNDPROC current_proc = reinterpret_cast<WNDPROC>(
		::GetWindowLongPtr(h_wnd_, GWLP_WNDPROC));
	if (current_proc == reinterpret_cast<WNDPROC>(WndProcStatic)) {
		::SetWindowLongPtr(h_wnd_, GWLP_WNDPROC,
			reinterpret_cast<LONG_PTR>(prev_wnd_proc_));
		prev_wnd_proc_ = nullptr;
	}
}