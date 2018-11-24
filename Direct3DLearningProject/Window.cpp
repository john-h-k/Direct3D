#include "Window.h"

#include "Direct3DObject.h"

Window::Window()
= default;

Window::~Window()
= default;

bool Window::Initialize(HINSTANCE hInstance, const int ShowWnd, const int width, const int height, bool windowed)
{
	WNDCLASSEX windowClass;

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIconW(nullptr, IDI_WINLOGO);
	windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 10);
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = WndClassName;
	windowClass.hIconSm = LoadIconW(nullptr, IDI_WINLOGO);

	if (!RegisterClassEx(&windowClass))
	{
		MessageBoxA(nullptr, "Error initializing window", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	hwnd = CreateWindowEx(
		0,
		WndClassName,
		L"Main Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!hwnd)
	{
		MessageBoxA(nullptr, "Error initializing window", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);

	return true;
}

int Window::MessageLoop(IUpdateable &updateable) const
{
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));

	while (true)
	{
		if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			updateable.Update();
		}
	}

	return static_cast<int>(msg.wParam);
}

LRESULT Window::WndProc(HWND hwnd, const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			if (MessageBoxA(nullptr, "Confirm exit", "Exit", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				DestroyWindow(hwnd);
			}
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}