#include <windows.h>
#include "Window.h"
#include "Direct3DObject.h"

const int Width = 800;
const int Height = 600;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, const int nShowCmd)
{
	auto mainWindow = Window();

	if (!mainWindow.Initialize(hInstance, nShowCmd, Width, Height, true))
	{
		MessageBox(nullptr, "Failed Window Initialization", "Error", MB_OK);
		return 0;
	}

	auto graphics = Direct3DObject();

	if (!graphics.InitializeDirect3D11App(hInstance, Width, Height, mainWindow.GetHandle()))
	{
		MessageBox(nullptr, "Error initializing Direct3D11", "Error", MB_OK);
		return 0;
	}

	if (!graphics.InitializeScene())
	{
		MessageBox(nullptr, "Error initializing scene", "Error", MB_OK);
		return 0;
	}

	mainWindow.MessageLoop(static_cast<IUpdateable&>(graphics));

	graphics.ReleaseObjects();

	return 0;
}
