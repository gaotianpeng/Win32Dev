#include "window_map.h"
#include "application.h"

Window* WindowMap::GetWindow(HWND hwnd) const {
	auto it = window_map_.find(hwnd);
	return it != window_map_.end() ? it->second: nullptr;
}

void WindowMap::Add(HWND hwnd, Window* window) {
	if (hwnd && !GetWindow(hwnd)) {
		window_map_.insert(std::make_pair(hwnd, window));
	}
}

void WindowMap::Clear() {
	for (const auto& elem : window_map_) {
		const HWND hwnd = elem.first;
		if (::IsWindow(hwnd)) {
			::DestroyWindow(hwnd);
		}
	}
}

void WindowMap::Remove(HWND hwnd) {
	window_map_.erase(hwnd);
}

void WindowMap::Remove(Window* window) {
	for (auto it = window_map_.begin(); it != window_map_.end(); ++it) {
		if (window == it->second) {
			window_map_.erase(it);
			return;
		}
	}
}

WindowMap& WindowMap::Instance() {
	return App::Instance().window_map_;
}