#pragma once

//Include and link appropriate libraries and headers//
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "IUpdateable.h"

class Renderer final : public IUpdateable
{
public:
	Renderer();
	Renderer(const Renderer& renderer) = delete;
	Renderer(Renderer&& renderer) = default;
	~Renderer();

	Renderer& operator = (const Renderer& renderer) = delete;
	Renderer& operator = (Renderer&& renderer) = default;

	bool InitializeDirect3D11App(HINSTANCE hInstance, int width, int height, HWND windowHandle);
	void Release();
	bool InitializeScene();
	void UpdateScene();
	void DrawScene() const;
	void Update() override;

private:
	bool CheckFailW(HRESULT hr, LPCWSTR str) const;
	bool CheckFailA(HRESULT hr, LPCSTR str) const;

	bool initialized;
	bool released;

	HWND window{};
	float height;
	float width;

	// DO NOT DELETE THESE OBJECTS USING "delete"!!
	// They are COM objects and will destroy themselves
	// once they have no references left to them. Just call "Release()"
	// when you are finished with them

	IDXGISwapChain* swapChain{};
	ID3D11Device* device{};
	ID3D11DeviceContext* context{};
	ID3D11RenderTargetView* renderTargetView{};

	ID3D11Buffer* triangularVertexBuffer{};
	ID3D11VertexShader* vertexShader{};
	ID3D11PixelShader* pixelShader{};
	ID3DBlob* vertexShaderBuffer{};
	ID3DBlob* pixelShaderBuffer{};
	ID3D11InputLayout* vertexLayout{};

	D3D11_INPUT_ELEMENT_DESC layoutArray[2]{};
	UINT layoutElementCount;
};
