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

	WPARAM MessageLoop(IUpdateable & updateable) const;
private:
	LPCWSTR WndClassName = L"Window";
	HWND hwnd = nullptr;

	LRESULT static CALLBACK WndProc(HWND hwnd, UINT msg,
		WPARAM wParam, LPARAM lParam);
};
