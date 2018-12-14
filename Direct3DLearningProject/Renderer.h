#pragma once

//Include and link appropriate libraries and headers//
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "IUpdateable.h"
#include <wrl/client.h>

namespace FactaLogicaSoftware
{
	class Renderer final : public IUpdateable
	{
	public:
		Renderer(const Renderer& renderer) = delete;
		~Renderer();

		Renderer& operator = (const Renderer& renderer) = delete;
		Renderer& operator = (Renderer&& renderer) = default;

		static Renderer Create(HINSTANCE hInstance, int width, int height, HWND windowHandle);
		bool InitializeScene();
		void UpdateScene();
		void DrawScene() const;
		void Update(double secs) override;

		bool Successful() const { return !initFailed; }

	private:
		Renderer();
		Renderer(Renderer&& renderer) = default;

#ifdef UNICODE
		bool CheckFailW(HRESULT hr, LPCWSTR str) const;
#else
		bool CheckFailA(HRESULT hr, LPCSTR str) const;
#endif

		bool initialized;
		bool initFailed;

		HWND window {};
		float height;
		float width;

		// COM objects

		Microsoft::WRL::ComPtr<ID3D11Device>			device {};
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>		context {};
		Microsoft::WRL::ComPtr<IDXGISwapChain>			swapChain{};
		Microsoft::WRL::ComPtr<ID3D11InputLayout>		vertexLayout {};
		Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader {};
		Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader {};
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView {};
		Microsoft::WRL::ComPtr<ID3DBlob>				vertexShaderBuffer {};
		Microsoft::WRL::ComPtr<ID3DBlob>				pixelShaderBuffer {};
		Microsoft::WRL::ComPtr<ID3D11Buffer>			squareIndexBuffer {};
		Microsoft::WRL::ComPtr<ID3D11Buffer>			squareVertexBuffer {};
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer{};
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView{};

		D3D_FEATURE_LEVEL 								featureLevel;
		D3D11_INPUT_ELEMENT_DESC						layoutArray[2] {};
		UINT											layoutElementCount;
	};
	
} 