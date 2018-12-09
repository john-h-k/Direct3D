#pragma once
#include <windows.h>
#include <vector>
#include <memory>

#define HWND(a) static_cast<HWND>(a)
#define HWND_FROM_UP(a) static_cast<HWND>(a.get())
#define VOID_HWND(a) (static_cast<void *>(a))

class IUpdateable;

struct WinHandleDeleter final
{
	bool* destroyed;

	explicit WinHandleDeleter(bool* destroyed) : destroyed(destroyed)
	{}

	void operator ()(HWND handle) const noexcept;
};

using UniqueWindowHandle = std::unique_ptr<std::remove_pointer_t<HWND>, WinHandleDeleter>;

class Window final
{
public:
	Window();
	Window(const Window&) = delete;
	Window(Window&&) = default;
	~Window();

	Window& operator = (const Window&) = delete;
	Window& operator = (Window&&) = default;

	bool Initialize(HINSTANCE hInstance, int ShowWnd,
		int width, int height, bool windowed);

	auto GetHandle() const { return HWND_FROM_UP(windowHandle); }

	 WPARAM EnterMessageLoop(IUpdateable& updateable);
private:
	bool destroyed;
	static std::vector<Window*> windows;

	SIZE_T windowIndex;

	LARGE_INTEGER interval;
	LARGE_INTEGER frequency;

	LPCWSTR WndClassName = L"Window";
	UniqueWindowHandle windowHandle;

	LRESULT static CALLBACK WndProc(HWND windowHandle, UINT msg,
		WPARAM wParam, LPARAM lParam);
};
