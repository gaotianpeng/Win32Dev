#ifndef __APP_H
#define __APP_H

#include <windows.h>
#include "window_map.h"

class App {
public:
	friend class WindowMap;

	App();
	virtual ~App();

	virtual BOOL InitInstance();
	BOOL InitCommonControls(DWORD flags) const;
	virtual int MessageLoop();
	virtual void PostQuitMessage(int exit_code = 0);
	virtual int Run();

private:
	static App& Instance(App* app = nullptr);

private:
	HINSTANCE instance_ = NULL;
	WindowMap window_map_;
};

#endif // __APP_H


