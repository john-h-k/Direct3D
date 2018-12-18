#include "Window.h"

#include "Renderer.h"
#include <stdexcept>
#include <tchar.h>
#include "DataTypes.h"

namespace FactaLogicaSoftware
{
#define DESIRED_INTERVAL_MS ((1000 / 60))
	// TODO make based off thread ID as well
#define VERIFY_HANDLE(HANDLE) 	if\
	(publicHandle && !IsWindow(windowHandle.get())) { throw std::runtime_error("Window destroyed outside of object"); }

	std::vector<Window*> Window::windows;
	std::map<HWND, WndProcException> Window::exceptions;

	Window::Window(HINSTANCE hInstance, int ShowWnd, int width, int height, LPCWSTR name, bool windowed)
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		frequency = freq.QuadPart / 1000.0;

		// Add this window to the static array of all windows,
		// and get the index of this
		windows.push_back(this);

		WndClassName = name;

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
		windowClass.lpszClassName = name;
		windowClass.hIconSm = LoadIconW(nullptr, IDI_WINLOGO);

		if (!RegisterClassExW(&windowClass))
		{
			MessageBoxW(nullptr, L"Error initializing window", L"Error", MB_OK | MB_ICONERROR);
			initFailed = true;
		}

		windowHandle = UniqueWindowHandle(CreateWindowExW(
			0,
			name,
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
			initFailed = true;
		}

		ShowWindow(HWND_FROM_UP(windowHandle), ShowWnd);
		UpdateWindow(HWND_FROM_UP(windowHandle));
	}

	Window::~Window()
	{
		if (publicHandle && !IsWindow(windowHandle.get()))
		{
			destroyed = true;
		}

		if (!destroyed)
		{
			// What to do if destroy failed? TODO
			destroyed = ::DestroyWindow(HWND_FROM_UP(windowHandle));
		}
		// Remove current instance from static
		windows.erase(windows.begin() + windowIndex);

	}

	WPARAM Window::EnterMessageLoop(IUpdateable& updateable) const
	{
		VERIFY_HANDLE(windowHandle);

		MSG msg;

		ZeroMemory(&msg, sizeof(MSG));

		LARGE_INTEGER interval;
		LARGE_INTEGER delta;

		QueryPerformanceCounter(&interval);

		const auto result = exceptions.find(this->windowHandle.get());
		const auto exception = result == exceptions.end() 
			? WndProcException(result->second.exceptionThrown, result->second.value.release()) 
			: WndProcException(false, nullptr);

		while (true)
		{
			if (exception.exceptionThrown)
			{
				// Manage exception
				throw std::exception(*exception.value);
			}

			if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT) { break; };

				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
			else
			{
				QueryPerformanceCounter(&delta);
				Sleep(static_cast<DWORD>(DESIRED_INTERVAL_MS - ((delta.QuadPart - interval.QuadPart) / frequency / 1000.0)));  // NOLINT
				updateable.Update((delta.QuadPart - interval.QuadPart) / frequency);
				QueryPerformanceCounter(&interval);
			}
		}

		return msg.wParam;
	}

	void Window::DestroyWindow(HWND handle)
	{
		Window* ref = GetWindowPtrFromHandle(handle);
		if (!ref)
		{
			// Function is called from WndProc, which can't catch
			exceptions[handle] = WndProcException(true, new std::runtime_error("No corresponding handle found for window"));
		}
		ref->destroyed = static_cast<bool>(::DestroyWindow(handle));
	}

	LRESULT Window::WndProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				if (CONFIRM_EXIT)
				{
					DestroyWindow(windowHandle);
				}
			}
			return 0;

		case WM_CLOSE:
			if (CONFIRM_EXIT)
			{
				DestroyWindow(windowHandle);
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
