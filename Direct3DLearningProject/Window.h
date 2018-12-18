#pragma once
#include <windows.h>
#include <vector>
#include <memory>
#include "IUpdateable.h"
#include <map>

#define HWND(a) (static_cast<HWND>(a))
#define HWND_FROM_UP(a) (static_cast<HWND>(a.get()))
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

		auto GetHandle() const { return HWND_FROM_UP(windowHandle); }
		auto IsDestroyed() const { return destroyed; }

		WPARAM EnterMessageLoop(IUpdateable& updateable) const;

		bool Successful() const { return !initFailed; }
		void SetHandleAccess(bool _publicHandle) { publicHandle = _publicHandle; }
		bool IsHandlePublic() const { return publicHandle;  }

	private:
		static void DestroyWindow(HWND handle);
		static std::map<HWND, WndProcException> exceptions;

		bool publicHandle = false;
		bool destroyed = false;
		bool initFailed = false;
		static std::vector<Window*> windows;

		SIZE_T windowIndex{};

		double frequency;

		LPCWSTR WndClassName = L"Window";
		UniqueWindowHandle windowHandle{
			nullptr,
			WinHandleDeleter(&destroyed)
		};

		static Window* GetWindowPtrFromHandle(HWND handle);
		static LRESULT CALLBACK WndProc(HWND windowHandle, UINT msg,
		                                WPARAM wParam, LPARAM lParam);
	};
}
