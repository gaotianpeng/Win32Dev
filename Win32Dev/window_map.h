#ifndef __WINDOW_MAP_H
#define __WINDOW_MAP_H

#include <map>
#include <windows.h>

class Window;

class WindowMap {
public:
	Window* GetWindow(HWND hwnd) const;

	void Add(HWND hwnd, Window* window);
	void Clear();
	void Remove(HWND hwnd);
	void Remove(Window* window);

	static WindowMap& Instance();
	
private:
	std::map<HWND, Window*> window_map_;
};



#endif // __WINDOW_MAP_H