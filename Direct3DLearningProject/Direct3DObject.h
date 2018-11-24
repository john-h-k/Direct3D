#pragma once

//Include and link appropriate libraries and headers//
#pragma comment(lib, "d3d11.lib")

#include <windows.h>
#include <d3d11.h>
#include "IUpdateable.h"

class Direct3DObject : public IUpdateable
{
public:
	Direct3DObject();
	~Direct3DObject();

	bool InitializeDirect3D11App(HINSTANCE hInstance, int width, int height, HWND hwnd);
	void ReleaseObjects() const;
	bool InitializeScene();
	void UpdateScene();
	void DrawScene() const;
	void Update() override;

private:
	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11RenderTargetView* renderTargetView;

	D3D11_INPUT_ELEMENT_DESC* layoutArray;
	UINT layoutElementCount;

	float red;
	float green;
	float blue;

	int colorModRed;
	int colorModGreen;
	int colorModBlue;
};
