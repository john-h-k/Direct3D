#include "Direct3DObject.h"
#include <string>
#include <sstream>
#include "Vertex.h"
#include <minwinbase.h>
#include <atlstr.h>
#include <fstream>
#include <comdef.h>

#define MESSAGE_FAILED(HR, STRING) CheckFail(HR, STRING)

using DIRECTX10COLOR = float[4];

Direct3DObject::Direct3DObject() :
	height(0),
	width(0)
{
	layoutArray[0] =
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0};

	layoutArray[1] =
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0};

	layoutElementCount = ARRAYSIZE(layoutArray);
}

Direct3DObject::~Direct3DObject()
= default;

bool Direct3DObject::InitializeDirect3D11App(HINSTANCE hInstance, const int width, const int height, HWND hwnd)
{
	window = hwnd;
	this->height = static_cast<float>(height);
	this->width = static_cast<float>(width);

	HRESULT hr;

	// Declare the buffer description
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

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

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	// Create the swap chain description
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = window;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Create the device and swap chain using the descriptions
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&swapChain,
		&device,
		nullptr,
		&context
	);

	if (MESSAGE_FAILED(hr, L"Fatal error occured in creation of Device and Swap Chain")) return false;

	// Declare and create the back buffer using the descriptions
	ID3D11Texture2D* BackBuffer;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&BackBuffer));

	if (MESSAGE_FAILED(hr, L"Fatal error occured in creation of Device or Swap Chain")) return false;

	hr = device->CreateRenderTargetView(BackBuffer, nullptr, &renderTargetView);
	BackBuffer->Release();

	if (MESSAGE_FAILED(hr, L"Fatal error occured in creation of Render Target View")) return false;

	context->OMSetRenderTargets(1, &renderTargetView, nullptr);

	return true;
}

void Direct3DObject::ReleaseObjects() const
{
	// Release all COM objects
	swapChain->Release();
	device->Release();
	context->Release();
	renderTargetView->Release();
	triangularVertexBuffer->Release();
	vertexShader->Release();
	pixelShader->Release();
	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();
	vertexLayout->Release();
}

bool Direct3DObject::InitializeScene()
{
	HRESULT hr;

	ID3D10Blob* errors;

	hr = D3DCompileFromFile(
		L"Effects.hlsl",
		nullptr,
		nullptr,
		"VS",
		"vs_4_0",
		0,
		0,
		&vertexShaderBuffer,
		&errors);

	if (errors)
	{
		std::ofstream file;
		file.open("log.txt", std::ios_base::app);
		file << "Shader compile error message: "
			<< static_cast<const char *>(errors->GetBufferPointer());
		file.close();
	}

	if (MESSAGE_FAILED(hr, L"Fatal error occured in compilation of shaders - check log file")) return false;

	hr = D3DCompileFromFile(
		L"Effects.hlsl",
		nullptr,
		nullptr,
		"PS",
		"ps_4_0",
		0,
		0,
		&pixelShaderBuffer,
		&errors);

	if (errors)
	{
		std::ofstream file;
		file.open("log.txt", std::ios_base::app);
		file << "Shader compile error message: "
			<< static_cast<const char *>(errors->GetBufferPointer());
		file.close();
	}

	if (MESSAGE_FAILED(hr, L"Fatal error occured in compilation of shaders - check log file")) return false;

	hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &vertexShader);

	if (MESSAGE_FAILED(hr, L"Fatal error occured during creation of the vertex shaders")) return false;

	hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &pixelShader);

	if (MESSAGE_FAILED(hr, L"Fatal error occured during creation of the vertex shaders")) return false;

	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	Vertex vertices[] =
	{
		Vertex(0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f),
		Vertex(0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f),
		Vertex(-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f)
	};

	const auto count = ARRAYSIZE(vertices);

	D3D11_BUFFER_DESC vertexBufferDesc;

	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = (sizeof(Vertex) * count) + (16 - ((sizeof(Vertex) * count) % 16));
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	assert(vertexBufferDesc.ByteWidth % 16 == 0);

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));

	vertexBufferData.pSysMem = vertices;

	if (device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, nullptr) != S_FALSE) return false;

	hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &triangularVertexBuffer);

	if (MESSAGE_FAILED(hr, L"Failure creating device vertex buffer")) return false;

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &triangularVertexBuffer, &stride, &offset);

	device->CreateInputLayout(layoutArray, layoutElementCount, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &vertexLayout);

	context->IASetInputLayout(vertexLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Height = height;
	viewport.Width = width;

	context->RSSetViewports(1, &viewport);

	return true;
}

void Direct3DObject::UpdateScene()
{
}

void Direct3DObject::DrawScene() const
{
	// Clear BackBuffer and change color
	const auto color = new DIRECTX10COLOR { 0.0f, 0.0f, 0.0f, 1.0f };
	
	context->ClearRenderTargetView(renderTargetView, color);

	context->Draw(3, 0);

	// Present the buffers
	swapChain->Present(0, 0);
}

void Direct3DObject::Update()
{
	UpdateScene();
	DrawScene();
}

bool Direct3DObject::CheckFail(const HRESULT hr, const LPCWSTR str) const
{
	if (FAILED(hr))
	{
		std::stringstream stringstream;

		stringstream << CW2A(str) << " HRESULT Error Code: "
		<< hr << " (" << CW2A(_com_error(hr).ErrorMessage()) << ")";

		MessageBoxA(window, stringstream.str().c_str(), "Error", MB_OK);

		return true;
	}
	return false;
}