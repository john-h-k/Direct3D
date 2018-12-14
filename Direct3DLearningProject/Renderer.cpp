#include <string>
#include <sstream>
#include <atlstr.h>
#include <fstream>
#include <comdef.h>
#include <iomanip>
#include <dxgi1_3.h>
#include <wrl.h>
#include "Renderer.h"
#include "Vertex.h"
#include "DataTypes.h"
#include "D3DCOLOR.h"

#define ZeroStruct(STRUCT) ZeroMemory(&STRUCT, sizeof(STRUCT))
#define DOUBLE_PRECISION 17 // 17 more liberal, 15 more conservative value
#define INTERVAL_DEBUG
#define REQUIRED_BYTE_WIDTH 16
#define PAD_UP(CURRENT_SIZE, PAD_UP_MOD_VALUE) ((CURRENT_SIZE) + (PAD_UP_MOD_VALUE - ((CURRENT_SIZE) % PAD_UP_MOD_VALUE)))


namespace FactaLogicaSoftware
{
	Renderer::Renderer() :
		initialized(false),
		height(0),
		width(0),
		layoutElementCount(0)

	{
		layoutArray[0] =
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		};

		layoutArray[1] =
		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			12,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		};

		layoutElementCount = ARRAYSIZE(layoutArray);
	}

	Renderer::~Renderer()
	{
	}

	Renderer Renderer::Create(HINSTANCE hInstance, int width, int height, HWND windowHandle)
	{
		auto renderer = Renderer();

		renderer.window = windowHandle;
		renderer.height = static_cast<float>(height);
		renderer.width = static_cast<float>(width);

		// Declare the buffer description
		DXGI_MODE_DESC bufferDesc;

		ZeroStruct(bufferDesc);

		// Create the buffer description
		bufferDesc.Width = width;
		bufferDesc.Height = height;
		bufferDesc.RefreshRate.Numerator = 60;
		bufferDesc.RefreshRate.Denominator = 1;
		bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// Declare the swap chain description
		DXGI_SWAP_CHAIN_DESC swapChainDesc;

		ZeroStruct(swapChainDesc);

		// Create the swap chain description
		swapChainDesc.BufferDesc = bufferDesc;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = renderer.window;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL permissibleFeatureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1
		};

		// Create the device and swap chain using the descriptions
		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			flags,
			permissibleFeatureLevels,
			ARRAYSIZE(permissibleFeatureLevels),
			D3D11_SDK_VERSION,
			&swapChainDesc,
			renderer.swapChain.GetAddressOf(),
			renderer.device.GetAddressOf(),
			&renderer.featureLevel,
			renderer.context.GetAddressOf()
		);

		assert(std::find(permissibleFeatureLevels, permissibleFeatureLevels + ARRAYSIZE(permissibleFeatureLevels), 0) != permissibleFeatureLevels + ARRAYSIZE(permissibleFeatureLevels));

		STATIC_MSG_FAIL_RET_FAIL(hr, L"Fatal error occured in creation of Device and Swap Chain");

		// Declare and create the back buffer using the descriptions
		Microsoft::WRL::ComPtr<ID3D11Texture2D> BackBuffer;
		hr = renderer.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(BackBuffer.GetAddressOf()));

		STATIC_MSG_FAIL_RET_FAIL(hr, L"Fatal error occured in creation of Device or Swap Chain");

		hr = renderer.device->CreateRenderTargetView(BackBuffer.Get(), nullptr, renderer.renderTargetView.GetAddressOf());

		STATIC_MSG_FAIL_RET_FAIL(hr, L"Fatal error occured in creation of Render Target View");

		renderer.context->OMSetRenderTargets(1, renderer.renderTargetView.GetAddressOf(), nullptr);

		renderer.initialized = true;

		return renderer;
	}

	bool Renderer::InitializeScene()
	{
		HRESULT hr;

		Microsoft::WRL::ComPtr<ID3DBlob> errors;

		hr = D3DCompileFromFile(
			L"Effects.hlsl",
			nullptr,
			nullptr,
			"VS",
			"vs_4_0",
			0,
			0,
			vertexShaderBuffer.GetAddressOf(),
			errors.GetAddressOf());

		if (errors)
		{
			std::ofstream file;
			file.open("log.txt", std::ios_base::app);
			file << "Shader compile error message: "
				<< static_cast<const char *>(errors->GetBufferPointer());
			file.close();
			return false;
		}

		MSG_FAIL_RET_FALSE(hr, L"Fatal error occured in compilation of shaders - check log file");

		hr = D3DCompileFromFile(
			L"Effects.hlsl",
			nullptr,
			nullptr,
			"PS",
			"ps_4_0",
			0,
			0,
			pixelShaderBuffer.GetAddressOf(),
			errors.GetAddressOf());

		if (errors)
		{
			std::ofstream file;
			file.open("log.txt", std::ios_base::app);
			file << "Shader compile error message: "
				<< static_cast<const char *>(errors->GetBufferPointer());
			file.close();
			return false;
		}

		MSG_FAIL_RET_FALSE(hr, L"Fatal error occured in compilation of shaders - check log file");

		hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &vertexShader);

		MSG_FAIL_RET_FALSE(hr, L"Fatal error occured during creation of the vertex shaders");

		hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &pixelShader);

		MSG_FAIL_RET_FALSE(hr, L"Fatal error occured during creation of the vertex shaders");

		context->VSSetShader(vertexShader.Get(), nullptr, 0);
		context->PSSetShader(pixelShader.Get(), nullptr, 0);

		DWORD indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};

		D3D11_BUFFER_DESC indexBufferDesc;

		ZeroStruct(indexBufferDesc);

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(indices);
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferData;

		indexBufferData.pSysMem = indices;
		hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, squareIndexBuffer.GetAddressOf());

		Vertex vertices[] =
		{
			Vertex(-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f),
			Vertex(-0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f),
			Vertex(0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f),
			Vertex(0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f),
		};

		D3D11_BUFFER_DESC vertexBufferDesc;

		ZeroStruct(vertexBufferDesc);

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = PAD_UP(sizeof(vertices), REQUIRED_BYTE_WIDTH);
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroStruct(vertexBufferData);

		vertexBufferData.pSysMem = vertices;

#ifdef DEBUG
		if (device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, nullptr) != S_FALSE) return false;
#endif

		hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &squareVertexBuffer);

		MSG_FAIL_RET_FALSE(hr, L"Failure creating device vertex buffer");

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, squareVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(squareIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		device->CreateInputLayout(layoutArray, layoutElementCount, vertexShaderBuffer->GetBufferPointer(),
			vertexShaderBuffer->GetBufferSize(), vertexLayout.GetAddressOf());

		context->IASetInputLayout(vertexLayout.Get());
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D11_VIEWPORT viewport;

		ZeroStruct(viewport);

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Height = height;
		viewport.Width = width;

		context->RSSetViewports(1, &viewport);

		return true;
	}

	void Renderer::UpdateScene()
	{
	}

	void Renderer::DrawScene() const
	{
		// Clear BackBuffer and change color
		auto color = D3DXCOLOR{ 0.0f, 0.0f, 0.0f, 1.0f };

		context->ClearRenderTargetView(renderTargetView.Get(), GetFloatPtrFromColor(color));

		context->DrawIndexed(6, 0, 0);

		// Present the buffers
		swapChain->Present(0, 0);
	}

	void Renderer::Update(double secs)
	{
#ifdef INTERVAL_DEBUG
		std::wstringstream stream;
		stream << "Interval time (potentially delayed by debug - un-define INTERVAL_DEBUG to change)";
		if (!(0 <= 1e-17 * std::abs(secs))) stream << std::setprecision(DOUBLE_PRECISION) << std::fixed << secs << std::endl;
		else stream << "INTERVAL TO SHORT TO MEASURE WITH QPC" << std::endl;
		OutputDebugStringW(stream.str().c_str());
#endif
		UpdateScene();
		DrawScene();
	}

#ifdef UNICODE
	bool Renderer::CheckFailW(const HRESULT hr, const LPCWSTR str) const
	{
		if (FAILED(hr))
		{
			std::wstringstream stringstream;

			stringstream << CW2A(str) << " HRESULT Error Code: "
				<< hr << " (" << CW2A(_com_error(hr).ErrorMessage()) << ")";

			MessageBoxW(window, stringstream.str().c_str(), L"Error", MB_OK);

			return true;
		}
		return false;
	}
#else
	bool Renderer::CheckFailA(const HRESULT hr, const LPCSTR str) const
	{
		if (FAILED(hr))
		{
			std::stringstream stringstream;

			stringstream << str << " HRESULT Error Code: "
				<< hr << " (" << _com_error(hr).ErrorMessage() << ")";

			MessageBoxA(window, stringstream.str().c_str(), "Error", MB_OK);

			return true;
		}
		return false;
	}
#endif
}
