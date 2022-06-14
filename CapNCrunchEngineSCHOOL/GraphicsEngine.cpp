#include "GraphicsEngine.h"
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include "Primitive.h"
#include "Vertex.h"

#include <array>

GraphicsEngine::GraphicsEngine() = default;
GraphicsEngine::~GraphicsEngine() = default;

bool GraphicsEngine::Init(int aHeight, int aWidth, HWND aWindowHandle)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = aWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;

	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr, 
		0,
		nullptr, 
		0, 
		D3D11_SDK_VERSION, 
		&swapChainDesc,
		&mySwapChain, 
		&myDevice, 
		nullptr, 
		&myContext);

	if (FAILED(result))
		return false;

	ID3D11Texture2D* backBufferTexture;
	result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	if (FAILED(result)) return false;

	result = myDevice->CreateRenderTargetView(backBufferTexture, nullptr, &myRenderTarget);
	if (FAILED(result)) return false;

	myContext->OMSetRenderTargets(1, myRenderTarget.GetAddressOf(), nullptr);

	D3D11_TEXTURE2D_DESC textureDesc = {};
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();

	D3D11_VIEWPORT viewport = {0};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(textureDesc.Width);
	viewport.Height = static_cast<float>(textureDesc.Height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	myContext->RSSetViewports(1, &viewport);

	myBGTriangle = new Primitive(myDevice.Get(), myContext.Get());
	{
		Vertex vertices[]
		{
			{ -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
			{  0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f},
			{  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f},
		};

		UINT indices[]
		{
			0, 1, 2
		};

		myBGTriangle->Init(vertices, std::size(vertices), indices, std::size(indices));
		myBGTriangle->SetVertexShader(L"ColorVS");
		myBGTriangle->SetPixelShader(L"CoolsinwavePS");
	}

	myCube = new Primitive(myDevice.Get(), myContext.Get());
	{
		Vertex vertices2[]
		{
			{  0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
			{  0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
			{ -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
			{ -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},

			{  0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 0.0f, 1.0f},
			{  0.5f,  0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 0.0f, 1.0f},
			{  0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f},
		};

		UINT indices2[]
		{
			0, 1, 2,
			2, 3, 0,

			0, 5, 4,
			4, 1, 0,

			0, 6, 5,
			0, 3, 6
		};

		myCube->Init(vertices2, std::size(vertices2), indices2, std::size(indices2));
		myCube->SetVertexShader(L"ColorVS");
		myCube->SetPixelShader(L"ColorPS");
	}

	return true;
}

void GraphicsEngine::Render()
{
	constexpr float color[4] = { 0.97f, 0.59f, 0.38f, 1.0f};
	myContext->ClearRenderTargetView(myRenderTarget.Get(), color);

	myBGTriangle->Render();
	myCube->Render();

	mySwapChain->Present(1, 0);
}