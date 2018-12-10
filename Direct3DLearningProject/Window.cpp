#include "Window.h"

#include "Renderer.h"
#include <stdexcept>
#include <tchar.h>
#include "DataTypes.h"

namespace FactaLogicaSoftware
{
	std::vector<Window*> Window::windows;

	Window::Window() : destroyed(false), windowHandle(nullptr, WinHandleDeleter(&destroyed))  // NOLINT
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		frequency = freq.QuadPart / 1000.0;
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
			max(width, 1),
			max(height, 1),
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

	WPARAM Window::EnterMessageLoop(IUpdateable& updateable) const
	{
		MSG msg;

		ZeroMemory(&msg, sizeof(MSG));

		LARGE_INTEGER interval;
		LARGE_INTEGER delta;

		QueryPerformanceCounter(&interval);

		while (true)
		{
			if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT) break;

				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
			else
			{
				QueryPerformanceCounter(&delta);
				updateable.Update((delta.QuadPart - interval.QuadPart) / frequency);
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
					// If exiting, a short for loop won't hurt performance
					// Link the HWND handle to the respective window using the static
					// array
					Window* ref = GetWindowPtrFromHandle(windowHandle);
					ref->destroyed = static_cast<bool>(DestroyWindow(windowHandle));
				}
			}
			return 0;

		case WM_CLOSE:
			if (CONFIRM_EXIT)
			{
				// If exiting, a short for loop won't hurt performance
				// Link the HWND handle to the respective window using the static
				// array
				Window* ref = GetWindowPtrFromHandle(windowHandle);
				ref->destroyed = static_cast<bool>(DestroyWindow(windowHandle));
			}

			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default:
			return DefWindowProcW(windowHandle, msg, wParam, lParam);
		}
	}

	Window* Window::GetWindowPtrFromHandle(HWND handle)
	{
		for (Window* window : windows)
		{
			if (window->GetHandle() == handle)
			{
				return window;
			}
		}

		return nullptr;
	}
}
