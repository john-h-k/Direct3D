#pragma once
#include <windows.h>
#include <vector>
#include <memory>
#include "IUpdateable.h"
#include <map>

#define HWND(a) (static_cast<HWND>(a))
#define HWND_FROM_UNIQUE_PTR(a) (static_cast<HWND>(a.get()))
#define VOID_HWND(a) (static_cast<void *>(a))

namespace FactaLogicaSoftware
{
	struct WinHandleDeleter final
	{
		bool* destroyed;

		explicit WinHandleDeleter(bool* destroyed) : destroyed(destroyed)
		{}

		void operator()(HWND handle) const noexcept
		{
			*destroyed = static_cast<bool>(DestroyWindow(handle));
		}

	};

	struct WndProcException final
	{
		WndProcException(bool thrown, std::exception* exception) :
			exceptionThrown(thrown), value(exception) {}

		bool exceptionThrown;
		std::unique_ptr<std::exception> value;
	};

	using UniqueWindowHandle = std::unique_ptr<std::remove_pointer_t<HWND>, WinHandleDeleter>;

	class Window final
	{
	public:
		Window(HINSTANCE hInstance, int ShowWnd, int width, int height, LPCWSTR name, bool windowed = false);
		Window(Window&& window) = delete;
		Window(const Window&) = delete;
		~Window();

		Window& operator = (const Window&) = delete;
		Window& operator = (Window&&) = delete;

		auto GetHandle() const noexcept { return HWND_FROM_UNIQUE_PTR(windowHandle); }
		auto IsDestroyed() const noexcept { return destroyed; }

		WPARAM EnterMessageLoop(IUpdateable& updateable) const;

		bool Successful() const noexcept { return !initFailed; }
		void SetHandleAccess(bool isHandlePublic) noexcept { publicHandle = isHandlePublic; }
		bool IsHandlePublic() const noexcept { return publicHandle; }

	private:
		static void DestroyWindow(HWND handle);
		static std::vector<Window*> windows;
		static std::map<HWND, WndProcException> exceptions;

		bool publicHandle = false;
		bool destroyed = false;
		bool initFailed = false;

		SIZE_T windowIndex{};

		double frequency;

		LPCWSTR WndClassName = L"Window";
		UniqueWindowHandle windowHandle
		{
			nullptr,
			WinHandleDeleter(&destroyed)
		};

		static Window* GetWindowPtrFromHandle(HWND handle);
		static LRESULT CALLBACK WndProc(HWND windowHandle, UINT msg,
			WPARAM wParam, LPARAM lParam);
	};
}
