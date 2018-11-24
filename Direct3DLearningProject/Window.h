#pragma once
#include <windows.h>
#include "IUpdateable.h"

class Window
{
public:
	Window();
	~Window();

	bool Initialize(HINSTANCE hInstance, int ShowWnd,
		int width, int height, bool windowed);

	HWND GetHandle() const { return hwnd; }

	int MessageLoop(IUpdateable &update) const;
private:
	LPCSTR WndClassName = "Window";
	HWND hwnd = nullptr;

	LRESULT static CALLBACK WndProc(HWND hwnd, UINT msg,
		WPARAM wParam, LPARAM lParam);
};

