#ifndef __WINDOW_H
#define __WINDOW_H

#include <windows.h>

class Window {
public:
	Window();
	Window(HWND hwnd);
	virtual ~Window();

	virtual HWND Create(HWND parent = nullptr);
	virtual HWND Create(DWORD ex_style, LPCWSTR class_name, LPCWSTR window_name, 
			DWORD style, int x, int y, int width, int height, HWND parent,
			HMENU menu, LPVOID param);

	virtual void PreCreate(CREATESTRUCT& cs);
	virtual void PreRegisterClass(WNDCLASSEX& wc);
	virtual BOOL PreTranslateMessage(MSG* msg);

	virtual void Destroy();

	HWND GetWindowHandle() const;
	void SetWindowHandle(HWND hwnd);

	BOOL BringWindowToTop() const;

protected:
	// msg handlers
	virtual BOOL OnCommand(WPARAM wparam, LPARAM LPARAM);
	virtual void OnContextMenu(HWND hwnd, POINT pt);
	virtual void OnCreate(HWND hwnd, LPCREATESTRUCT create_struct);
	virtual BOOL OnDestroy();
	virtual void OnDropFiles(HDROP drop_info);
	virtual void OnGetMinMaxInfo(LPMINMAXINFO mmi);
	virtual LRESULT OnMouseEvent(UINT msg, WPARAM wpara, LPARAM lparam);
	virtual void OnMove(LPPOINTS pts);
	virtual LRESULT OnNotify(int control_id, LPNMHDR nmh);
	virtual void OnPaint(HDC hdc, LPPAINTSTRUCT ps);
	virtual void OnSize(UINT msg, UINT type, SIZE size);
	virtual void OnTaskbarCallback(UINT msg, LPARAM lparam);
	virtual void OnTimer(UINT_PTR event_id);
	virtual void OnWindowPosChanging(LPWINDOWPOS wnd_pos);



	virtual LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	virtual LRESULT DefaultWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	CREATESTRUCT create_struct_;
	HINSTANCE instance_;
	WNDCLASSEX wnd_class_;
	WNDPROC prev_wnd_proc_;
	HFONT font_;

private:
	static LRESULT CALLBACK WndProcStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	BOOL RegisterClass(WNDCLASSEX& wc) const;
	void Subclass(HWND hwnd);
	void UnSubclass();

private:
	HWND h_parent_wnd_;
	HWND h_wnd_;

	static Window* current_window_;
};

#endif // __WINDOW_H

