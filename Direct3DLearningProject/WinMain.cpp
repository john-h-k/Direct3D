#include <windows.h>
#include "Window.h"
#include "Renderer.h"

const int Width = 800;
const int Height = 600;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, const int nShowCmd)
{
	auto mainWindow = Window();

	if (!mainWindow.Initialize(hInstance, nShowCmd, Width, Height, true))
	{
		MessageBoxW(nullptr, L"Failed Window Initialization", L"Error", MB_OK);
		return 0;
	}

	auto graphics = Renderer();

	if (!graphics.InitializeDirect3D11App(hInstance, Width, Height, mainWindow.GetHandle()))
	{
		MessageBoxW(nullptr, L"Error initializing Direct3D11", L"Error", MB_OK);
		return 0;
	}

	if (!graphics.InitializeScene())
	{
		MessageBoxW(nullptr, L"Error initializing scene", L"Error", MB_OK);
		return 0;
	}

	mainWindow.MessageLoop(static_cast<IUpdateable&>(graphics));

	graphics.ReleaseObjects();

	return 0;
}