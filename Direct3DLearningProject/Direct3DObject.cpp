#include "Direct3DObject.h"
#include <string>
#include <sstream>

using XDEPRECATEDCOLOR = float[4];

Direct3DObject::Direct3DObject() :
	swapChain(nullptr),
	device(nullptr),
	context(nullptr),
	renderTargetView(nullptr)
{
	D3D11_INPUT_ELEMENT_DESC tempLayoutArray[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	layoutArray = +tempLayoutArray;

	layoutElementCount = ARRAYSIZE(tempLayoutArray);

	red = 0.0f;
	green = 0.0f;
	blue = 0.0f;

	colorModRed = 1;
	colorModGreen = 1;
	colorModBlue = 1;
}

Direct3DObject::~Direct3DObject()
= default;

bool Direct3DObject::InitializeDirect3D11App(HINSTANCE hInstance, int width, int height, HWND hwnd)
{
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
	swapChainDesc.OutputWindow = hwnd;
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

	if (FAILED(hr))
	{
		std::stringstream stringstream;

		stringstream << "Fatal error occured in creation of Device or Swap Chain - HRESULT:  " << hr;

		MessageBoxA(hwnd, stringstream.str().c_str(), "Error", MB_OK);
		return false;
	}

	// Declare and create the back buffer using the descriptions
	ID3D11Texture2D* BackBuffer;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&BackBuffer));

	if (FAILED(hr))
	{
		std::stringstream stringstream;

		stringstream << "Fatal error occured in creation of Buffer - HRESULT: " << hr;

		MessageBoxA(hwnd, stringstream.str().c_str(), "Error", MB_OK);
		return false;
	}

	hr = device->CreateRenderTargetView(BackBuffer, nullptr, &renderTargetView);
	BackBuffer->Release();

	if (FAILED(hr))
	{
		std::stringstream stringstream;

		stringstream << "Fatal error occured in creation of Render Target View - HRESULT: " << hr;

		MessageBoxA(hwnd, stringstream.str().c_str(), "Error", MB_OK);
		return false;
	}

	context->OMSetRenderTargets(1, &renderTargetView, nullptr);

	return true;
}

void Direct3DObject::ReleaseObjects() const
{
	// Release all COM objects
	swapChain->Release();
	device->Release();
	context->Release();
}

bool Direct3DObject::InitializeScene()
{
	return true;
}

void Direct3DObject::UpdateScene()
{
	//Update the colors of our scene
	red += colorModRed * 0.00005f;
	green += colorModGreen * 0.00002f;
	blue += colorModBlue * 0.00001f;

	if (red >= 1.0f || red <= 0.0f)
		colorModRed *= -1;
	if (green >= 1.0f || green <= 0.0f)
		colorModGreen *= -1;
	if (blue >= 1.0f || blue <= 0.0f)
		colorModBlue *= -1;
}

void Direct3DObject::DrawScene() const
{
	// Clear BackBuffer and change color
	const auto color = new XDEPRECATEDCOLOR{ red, green, blue, 1.0f };

	context->ClearRenderTargetView(renderTargetView, color);

	// Present the buffers
	swapChain->Present(0, 0);
}

void Direct3DObject::Update()
{
	UpdateScene();
	DrawScene();
}