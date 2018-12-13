#include <windows.h>
#include <vld.h>
#include "Window.h"
#include "Renderer.h"
#include "ERRORCODES.h"

const int Width = 800;
const int Height = 600;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nShowCmd)
{
	// Create a window object. 
	auto mainWindow = FactaLogicaSoftware::Window::Create(hInstance, nShowCmd, Width, Height, L"Window");

	// Try to initialize the window using the WinMain args
	if (!mainWindow.Successful())
	{
		MessageBoxW(nullptr, L"Failed Window Initialization", L"Error", MB_OK);
		return FAIL_WINDOW_INIT;
	}

	// The Direct3D object used for rendering
	FactaLogicaSoftware::Renderer graphics = FactaLogicaSoftware::Renderer::Create(hInstance, Width, Height, mainWindow.GetHandle());

	// Try to initialize the renderer using the window
	if (!graphics.Successful())
	{
		MessageBoxW(nullptr, L"Error initializing Direct3D11", L"Error", MB_OK);
		return FAIL_DIRECT3D_INIT;
	}

	// Compile the shaders and initialize them
	if (!graphics.InitializeScene())
	{
		MessageBoxW(nullptr, L"Error initializing scene", L"Error", MB_OK);
		return FAIL_DIRECT3D_SCENE_INIT;
	}

	mainWindow.EnterMessageLoop(graphics);

	if (!mainWindow.IsDestroyed())
	{
		return FAIL_WINDOW_DESTROY;
	}

	return SUCCEED;
}