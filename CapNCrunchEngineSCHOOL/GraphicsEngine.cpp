#include "GraphicsEngine.h"
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#define REPORT_DX_WARNINGS

GraphicsEngine::GraphicsEngine() = default;
GraphicsEngine::~GraphicsEngine() = default;

bool GraphicsEngine::Init(int aHeight, int aWidth, HWND aWindowHandle)
{
	myVsyncEnabled = true;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = aWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;

	UINT creationFlags = 0;
#if defined _DEBUG && defined REPORT_DX_WARNINGS 
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr, 
		creationFlags,
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

	D3D11_TEXTURE2D_DESC textureDesc = {};
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();

	myContext->OMSetRenderTargets(1, myRenderTarget.GetAddressOf(), nullptr);

	D3D11_VIEWPORT viewport = {0};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(textureDesc.Width);
	viewport.Height = static_cast<float>(textureDesc.Height);
	viewport.Height = static_cast<float>(textureDesc.Height);
	viewport.MinDepth = 0.0f;
	viewport.MinDepth = 1.0f;
	myContext->RSSetViewports(1, &viewport);

	return true;
}

void GraphicsEngine::Render()
{
	constexpr float color[4] = { 0.97f, 0.59f, 0.38f, 1.0f};
	myContext->ClearRenderTargetView(myRenderTarget.Get(), color);

	mySwapChain->Present((myVsyncEnabled) ? 1 : 0, 0);
}