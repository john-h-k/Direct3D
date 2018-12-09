#include <windows.h>
#include "Window.h"
#include "Renderer.h"

const int Width = 800;
const int Height = 600;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, const int nShowCmd)
{
	// Create a window object. 
	auto mainWindow = Window();

	// Try to initialize the window using the WinMain args
	if (!mainWindow.Initialize(hInstance, nShowCmd, Width, Height, true))
	{
		MessageBoxW(nullptr, L"Failed Window Initialization", L"Error", MB_OK);
		return 0;
	}

	// The Direct3D object used for rendering
	Renderer graphics = Renderer();

	// Try to initialize the renderer using the window
	if (!graphics.InitializeDirect3D11App(hInstance, Width, Height, mainWindow.GetHandle()))
	{
		MessageBoxW(nullptr, L"Error initializing Direct3D11", L"Error", MB_OK);
		return 0;
	}

	// Compile the shaders and initialize them
	if (!graphics.InitializeScene())
	{
		MessageBoxW(nullptr, L"Error initializing scene", L"Error", MB_OK);
		return 0;
	}

	// Enter the message loop with our graphic object. Update() is called each loop
	mainWindow.EnterMessageLoop(static_cast<IUpdateable&>(graphics));

	// Direct3D release all
	graphics.Release();

	return 0;
}