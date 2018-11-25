#pragma once

//Include and link appropriate libraries and headers//
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
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
	bool CheckFail(HRESULT hr, LPCWSTR str) const;

	HWND window;
	float height;
	float width;

	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11RenderTargetView* renderTargetView;

	ID3D11Buffer* triangularVertexBuffer;
	ID3D11VertexShader* vertexShader{};
	ID3D11PixelShader* pixelShader{};
	ID3D10Blob* vertexShaderBuffer{};
	ID3D10Blob* pixelShaderBuffer{};
	ID3D11InputLayout* vertexLayout{};

	D3D11_INPUT_ELEMENT_DESC layoutArray[1];
	UINT layoutElementCount;

	float red;
	float green;
	float blue;

	int colorModRed;
	int colorModGreen;
	int colorModBlue;
};
