#include "Window.h"

#include "Renderer.h"
#include <cassert>
#include <stdexcept>
#include <tchar.h>
#include "DataTypes.h"
#include "Updateables.h"

#define CONFIRM_EXIT (MessageBoxW(nullptr, L"Are you sure you want to exit?", L"Confirm exit", MB_YESNO | MB_ICONQUESTION) == IDYES)

std::vector<Window*> Window::windows;

Window::Window() : destroyed(false), interval(), frequency(), windowHandle(nullptr, WinHandleDeleter(&destroyed))
{
	QueryPerformanceFrequency(&frequency);
	// Add this window to the static array of all windows,
	// and get the index of this
	windows.push_back(this);
	windowIndex = windows.size() - 1;
}

void WinHandleDeleter::operator()(HWND handle) const noexcept
{
	*destroyed = static_cast<bool>(DestroyWindow(handle));
}

Window::~Window()
{
	if (!destroyed)
	{
		// What to do if destroy failed? TODO
		destroyed = DestroyWindow(HWND_FROM_UP(windowHandle));
	}
	// Remove current instance from static
	windows.erase(windows.begin() + windowIndex);

}

bool Window::Initialize(HINSTANCE hInstance, const int ShowWnd, const int width, const int height, bool windowed)
{
	// The descriptor of this window
	WNDCLASSEX windowClass;

	windowClass.cbSize = sizeof(WNDCLASSEX); // Size of the struct
	windowClass.style = CS_HREDRAW | CS_VREDRAW; // Redraw when the window is moved/resized
	windowClass.lpfnWndProc = WndProc; // The function called for messages
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIconW(nullptr, IDI_WINLOGO);
	windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 10);
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = WndClassName;
	windowClass.hIconSm = LoadIconW(nullptr, IDI_WINLOGO);

	if (!RegisterClassExW(&windowClass))
	{
		MessageBoxW(nullptr, L"Error initializing window", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	windowHandle = UniqueWindowHandle(CreateWindowExW(
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
	), WinHandleDeleter(&destroyed));

	if (!windowHandle)
	{
		MessageBoxA(nullptr, "Error initializing window", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(HWND_FROM_UP(windowHandle), ShowWnd);
	UpdateWindow(HWND_FROM_UP(windowHandle));

	return true;
}

WPARAM Window::EnterMessageLoop(IUpdateable& updateable)
{
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));

	while (true)
	{
		if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TRACE(R"--(,"% *.*s",PEEK,0x%08x,"%s",0x%08x,0x%08x,0x%08x)--" "\n",
				0, 0, "", msg.hwnd, "Window HAS msg",
				msg.message, msg.wParam, msg.lParam);

			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else
		{
			QueryPerformanceCounter(&interval);
			updateable.Update(interval.QuadPart / static_cast<double>(frequency.QuadPart));
			QueryPerformanceCounter(&interval);
		}
	}

	return msg.wParam;
}

LRESULT Window::WndProc(HWND windowHandle, const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			if (CONFIRM_EXIT)
			{
				Window* ref = nullptr;
				// If exiting, a short for loop won't hurt performance
				// Link the HWND handle to the respective window using the static
				// array
				for (auto& window : windows)
				{
					if (window->GetHandle() == windowHandle)
					{
						ref = window;
					}
				}

				// If no window is found, something has gone wrong. Fatal error
				if (!ref)
				{
					throw std::logic_error("No corresponding window found for HWND");
				}
				ref->destroyed = static_cast<bool>(DestroyWindow(windowHandle));
			}
		}
		return 0;

	case WM_CLOSE:
		if (CONFIRM_EXIT) DestroyWindow(windowHandle);

		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProcW(windowHandle, msg, wParam, lParam);
	}
}