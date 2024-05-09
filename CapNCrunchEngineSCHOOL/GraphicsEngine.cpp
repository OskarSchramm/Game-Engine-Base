#include "GraphicsEngine.h"
#include <array>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include "Primitive.h"
#include "Vertex.h"

namespace CU
{
	template<class T>
	inline Matrix4x4<T> operator*(const Matrix4x4<T>& aLhs, const Matrix4x4<T>& aRhs)
	{
		Matrix4x4<T> m;
		for (size_t row = 0; row < 4; ++row)
			m.myRows[row] = aLhs.myRows[row] * aRhs;
		return m;
	}
}

GraphicsEngine::GraphicsEngine() = default;
GraphicsEngine::~GraphicsEngine() = default;

bool GraphicsEngine::Init(int aHeight, int aWidth, HWND aWindowHandle)
{
	aHeight; aWidth;
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = aWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;

	HRESULT result = S_FALSE;
	result = D3D11CreateDeviceAndSwapChain(
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

	//Viewport
	D3D11_TEXTURE2D_DESC textureDesc = {};
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();

	auto width = static_cast<float>(textureDesc.Width);
	auto height = static_cast<float>(textureDesc.Height);

	D3D11_VIEWPORT viewport = {0};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	myContext->RSSetViewports(1, &viewport);

	//DepthBuffer
	ID3D11Texture2D* depthBufferTexture;
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = (UINT)width;
	depthBufferDesc.Height = (UINT)height;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	result = myDevice->CreateTexture2D(&depthBufferDesc, nullptr, &depthBufferTexture);
	if (FAILED(result))
		return false;

	result = myDevice->CreateDepthStencilView(depthBufferTexture, nullptr, &myDepthStencilView);
	if (FAILED(result))
		return false;

	//DepthStencil 
	ID3D11DepthStencilState* depthStencilState;
	D3D11_DEPTH_STENCIL_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.DepthEnable = true;
	depthStencilViewDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilViewDesc.DepthFunc = D3D11_COMPARISON_LESS;

	myDevice->CreateDepthStencilState(&depthStencilViewDesc, &depthStencilState);

	depthBufferTexture->Release();

	myContext->OMSetRenderTargets(1, myRenderTarget.GetAddressOf(), myDepthStencilView.Get());
	myContext->OMSetDepthStencilState(depthStencilState, 0);

	//Camera
	myCamera.Init(CU::Vector3f{ 0.0f, 0.0f, -10.0f }, CU::Vector3f::Zero, 90.0f, CU::Vector2f{ width, height }, 0.01f, 1000.0f);

	//Create ConstantBuffers - objectbuffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(ObjectBuffer);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = myDevice->CreateBuffer(&bufferDesc, nullptr, &myObjectBuffer);
	if (FAILED(result))
		return false;

	//Create ConstantBuffers - framebuffer
	bufferDesc.ByteWidth = sizeof(FrameBuffer);
	result = myDevice->CreateBuffer(&bufferDesc, nullptr, &myFrameBuffer);
	if (FAILED(result))
		return false;

	//ADDED
	myCube = new Primitive(myDevice.Get(), myContext.Get());
	{
		Vertex vertices[]
		{
			{-0.5f,-0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
			{ 0.5f,-0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
			{ 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
			{-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
			{-0.5f,-0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
			{ 0.5f,-0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
			{ 0.5f, 0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
			{-0.5f, 0.5f,-0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
		};

		UINT indices[]
		{
			0, 3, 2, 2, 1, 0,
			1, 2, 6, 6, 5, 1,
			5, 6, 7, 7, 4, 5,
			4, 7, 3, 3, 0, 4,
			4, 0, 1, 1, 5, 4,
			3, 7, 6, 6, 2, 3
		};

		if (!myCube->Init(vertices, (UINT)std::size(vertices), indices, (UINT)std::size(indices)))
			return false;

		if (!myCube->SetVertexShader(L"shaders/Color_VS"))
			return false;

		if (!myCube->SetPixelShader(L"shaders/Color_PS"))
			return false;

		myCube->SetPosition({ 0.0f, 0.0f, 0.0f });
	}

	//ADDED
	myPyramid = new Primitive(myDevice.Get(), myContext.Get());
	{
		Vertex vertices[]
		{
			//Base
			{ -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f },
			{ -0.5f, -0.5f,-0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
			{  0.5f, -0.5f,-0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
			// top
			{ 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f},
		};

		UINT indices[]
		{
			//Bottom Face
			0, 2, 3,
			3, 1, 0,

			1, 4, 0,
			0, 4, 2,
			2, 4, 3,
			3, 4, 1,
		};

		if (!myPyramid->Init(vertices, (UINT)std::size(vertices), indices, (UINT)std::size(indices)))
			return false;

		if (!myPyramid->SetVertexShader(L"shaders/Color_VS"))
			return false;

		if (!myPyramid->SetPixelShader(L"shaders/Coolsinwave_PS"))
			return false;

		myPyramid->SetPosition({ 0.0f, 0.0f, 5.0f });
	}

	return true;
}

void GraphicsEngine::Render()
{
	constexpr float color[4] = { 0.97f, 0.59f, 0.38f, 1.0f };
	myContext->ClearRenderTargetView(myRenderTarget.Get(), color);
	myContext->ClearDepthStencilView(myDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//ADDED
	RenderPrimitive(myCube);
	RenderPrimitive(myPyramid);

	mySwapChain->Present(1, 0);
}

void GraphicsEngine::RenderPrimitive(Primitive* aPrimitive)
{
	D3D11_MAPPED_SUBRESOURCE mappedCBuffer = {};

	//Frame Buffer
	{
		FrameBuffer frameBufferData = {};
		auto worldToClip = CU::Matrix4x4f::GetFastInverse(myCamera.GetViewMatrix()) * myCamera.GetProjectionMatrix();

		frameBufferData.worldToClipMatrix = worldToClip;
		frameBufferData.totalTime = (float)myTimer.GetTotalTime();

		myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCBuffer);
		memcpy(mappedCBuffer.pData, &frameBufferData, sizeof(FrameBuffer));
		myContext->Unmap(myFrameBuffer, 0);
		myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	}

	//Object Buffer
	{
		ObjectBuffer objectBufferData = {};
		objectBufferData.objectMatrix = aPrimitive->GetModelMatrix();

		myContext->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCBuffer);
		memcpy(mappedCBuffer.pData, &objectBufferData, sizeof(ObjectBuffer));
		myContext->Unmap(myObjectBuffer, 0);
		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
	}

	aPrimitive->Render();
}

void GraphicsEngine::Update()
{
	myTimer.Update();

	myCamera.Update(myTimer.GetDeltaTime());
}