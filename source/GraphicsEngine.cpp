#include "GraphicsEngine.h"

//microsoft dx11
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

//cpc
#include "Primitive.h"
#include "Vertex.h"
#include "TextureTypes.h"
#include "CU/UtilityFunctions.hpp"

namespace
{
	typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;
#pragma warning(disable:4146)
	uint32_t pcg32_random_r(pcg32_random_t* rng)
	{
		uint64_t oldstate = rng->state;
		// Advance internal state
		rng->state = oldstate * 6364136223846793005ULL + (rng->inc | 1);
		// Calculate output function (XSH RR), uses old state for max ILP
		uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
		uint32_t rot = oldstate >> 59u;
		return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
	}

	// Returns a random float between 0.0 and 1.0
	float RandomFloat()
	{
		thread_local pcg32_random_t state;
		return (float)ldexp(pcg32_random_r(&state), -32);
	}

	void AddNoise(std::vector<float>& buffer, float amount)
	{
		for (int i = 0; i < buffer.size(); i++)
		{
			buffer[i] += (2.0f * RandomFloat() - 1.0f) * amount;
		}
	}

	std::vector<float> Upsample2X(const std::vector<float>& input, int resolution)
	{
		std::vector<float> output(2 * resolution * 2 * resolution);


		// corners
		output[0] = input[0];
		output[2 * resolution * (2 * resolution - 1)] = input[resolution * (resolution - 1)];
		output[2 * resolution - 1] = input[resolution - 1];
		output[2 * resolution * (2 * resolution - 1) + (2 * resolution - 1)] = input[resolution * (resolution - 1) + (resolution - 1)];

		for (int i = 0; i < resolution - 1; i++)
		{
			// edge where j = 0
			{
				float in0 = input[resolution * (i + 0)];
				float in1 = input[resolution * (i + 1)];

				output[2 * resolution * (2 * i + 1)] = 0.25f * (3.0f * in0 + 1.0f * in1);
				output[2 * resolution * (2 * i + 2)] = 0.25f * (1.0f * in0 + 3.0f * in1);
			}

			// interior
			for (int j = 0; j < resolution - 1; j++)
			{
				float in00 = input[resolution * (i + 0) + j + 0];
				float in01 = input[resolution * (i + 1) + j + 0];
				float in10 = input[resolution * (i + 0) + j + 1];
				float in11 = input[resolution * (i + 1) + j + 1];

				output[2 * resolution * (2 * i + 1) + 2 * j + 1] = 0.25f * 0.25f * (3.0f * 3.0f * in00 + 3.0f * 1.0f * in01 + 1.0f * 3.0f * in10 + 1.0f * 1.0f * in11);
				output[2 * resolution * (2 * i + 2) + 2 * j + 1] = 0.25f * 0.25f * (3.0f * 1.0f * in00 + 3.0f * 3.0f * in01 + 1.0f * 1.0f * in10 + 1.0f * 3.0f * in11);
				output[2 * resolution * (2 * i + 1) + 2 * j + 2] = 0.25f * 0.25f * (1.0f * 3.0f * in00 + 1.0f * 1.0f * in01 + 3.0f * 3.0f * in10 + 3.0f * 1.0f * in11);
				output[2 * resolution * (2 * i + 2) + 2 * j + 2] = 0.25f * 0.25f * (1.0f * 1.0f * in00 + 1.0f * 3.0f * in01 + 3.0f * 1.0f * in10 + 3.0f * 3.0f * in11);
			}

			// edge where j = is resolution-1
			{
				float in0 = input[resolution * (i + 0) + resolution - 1];
				float in1 = input[resolution * (i + 1) + resolution - 1];

				output[2 * resolution * (2 * i + 1) + 2 * resolution - 1] = 0.25f * (3.0f * in0 + 1.0f * in1);
				output[2 * resolution * (2 * i + 2) + 2 * resolution - 1] = 0.25f * (1.0f * in0 + 3.0f * in1);
			}
		}

		for (int j = 0; j < resolution - 1; j++)
		{
			// edge where i = 0
			{
				float in0 = input[j + 0];
				float in1 = input[j + 1];

				output[(2 * j + 1)] = 0.25f * (3.0f * in0 + 1.0f * in1);
				output[(2 * j + 2)] = 0.25f * (1.0f * in0 + 3.0f * in1);
			}

			// edge where i = is resolution-1
			{
				float in0 = input[resolution * (resolution - 1) + j + 0];
				float in1 = input[resolution * (resolution - 1) + j + 1];

				output[2 * resolution * (2 * resolution - 1) + (2 * j + 1)] = 0.25f * (3.0f * in0 + 1.0f * in1);
				output[2 * resolution * (2 * resolution - 1) + (2 * j + 2)] = 0.25f * (1.0f * in0 + 3.0f * in1);
			}
		}

		return output;
	}
}

GraphicsEngine::GraphicsEngine() = default;
GraphicsEngine::~GraphicsEngine() = default;

bool GraphicsEngine::Init(int aWidth, int aHeight, HWND aWindowHandle)
{
	if (!CreateDeviceAndSwapChain(&aWindowHandle))
		return false;

	if (!CreateViewPort(aWidth, aHeight))
		return false;

	if (!CreateDepthBufferAndStencil(aWidth, aHeight))
		return false;

	//NO need
	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer);

	if (!CreateConstantBuffers())
		return false;

	if (!CreateSamplerState())
		return false;

	myCamera.Init(CU::Vector3f{ 0.0f, 5.0f, -5.0f }, {45.0f, 45.0f, 0.0f}, 90.0f, CU::Vector2f{ (float)aWidth, (float)aHeight }, 0.01f, 1000.0f);

	myLight.SetDirectionalLight({ 0.5f, -0.5f, 0.0f }, {0.1f, 0.1f, 0.1, 0.5f});
	myLight.SetAmbientLight({ 0.9f, 0.35f, 0.25f, 1.0f }, { 0.65f, 0.5f, 0.37f, 1.0f }); 

	myCubeMap.Init(myDevice.Get(), myContext.Get(), L"bin/data/textures/cube_1024_preblurred_angle3_Skansen3.dds");
	myCubeMap.BindPS(myContext.Get(), TextureType::CubeMap);

	GenerateTerrain();

	CreateWaterRenderTarget(aWidth, aHeight);
	CreateFFCRasterizer();
	GeneratePlane(20, myWaterHeight);

	D3D11_MAPPED_SUBRESOURCE mappedCBuffer = {};
	ZeroMemory(&mappedCBuffer, sizeof(D3D11_MAPPED_SUBRESOURCE));
	//SettingsBuffer
	{
		mySettingsData.myResolutionHeight = aWidth;
		mySettingsData.myResolutionWidth  = aHeight;
		mySettingsData.myClipPlaneHeight = 0; //IDK why
		mySettingsData.padding = 0;

		myContext->Map(mySettingsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCBuffer);
		memcpy(mappedCBuffer.pData, &mySettingsData, sizeof(SettingsBuffer));
		myContext->Unmap(mySettingsBuffer, 0);
		myContext->PSSetConstantBuffers(3, 1, &mySettingsBuffer);
		myContext->VSSetConstantBuffers(3, 1, &mySettingsBuffer);
	}

	return true;
}

bool GraphicsEngine::CreateDeviceAndSwapChain(HWND* aWindowHandle)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = *aWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;

	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mySwapChain,
		&myDevice,
		nullptr,
		&myContext);

	if (FAILED(result)) return false;

	return true;
}

bool GraphicsEngine::CreateViewPort(int& aWidth, int& aHeight)
{
	HRESULT result;

	ID3D11Texture2D* backBufferTexture;
	result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	if (FAILED(result)) return false;

	result = myDevice->CreateRenderTargetView(backBufferTexture, nullptr, &myBackBuffer);
	if (FAILED(result)) return false;

	//Viewport
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width  = aWidth;
	textureDesc.Height = aHeight;
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();

	aWidth = static_cast<float>(textureDesc.Width);
	aHeight = static_cast<float>(textureDesc.Height);

	D3D11_VIEWPORT viewport = { 0 };
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = aWidth;
	viewport.Height = aHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	myContext->RSSetViewports(1, &viewport);

	return true;
}

bool GraphicsEngine::CreateDepthBufferAndStencil(const UINT aWidth, const UINT aHeight)
{
	HRESULT result;
	ID3D11DepthStencilState* depthStencilState = nullptr;

	//DepthBuffer
	ID3D11Texture2D* depthBufferTexture;
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = aWidth;
	depthBufferDesc.Height = aHeight;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	result = myDevice->CreateTexture2D(&depthBufferDesc, nullptr, &depthBufferTexture);
	if (FAILED(result))
		return false;

	result = myDevice->CreateDepthStencilView(depthBufferTexture, nullptr, &myDepthBuffer);
	if (FAILED(result)) 
		return false;

	//DepthStencil 
	D3D11_DEPTH_STENCIL_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.DepthEnable = true;
	depthStencilViewDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilViewDesc.DepthFunc = D3D11_COMPARISON_LESS;

	result = myDevice->CreateDepthStencilState(&depthStencilViewDesc, &depthStencilState);
	if (FAILED(result)) return false;

	myContext->OMSetDepthStencilState(depthStencilState, 0);

	depthBufferTexture->Release();

	return true;
}

bool GraphicsEngine::CreateConstantBuffers()
{
	HRESULT result;

	//Objectbuffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(ObjectBuffer);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = myDevice->CreateBuffer(&bufferDesc, nullptr, &myObjectBuffer);
	if (FAILED(result))
		return false;

	//Framebuffer
	bufferDesc.ByteWidth = sizeof(FrameBuffer);
	result = myDevice->CreateBuffer(&bufferDesc, nullptr, &myFrameBuffer);
	if (FAILED(result))
		return false;

	//Lightbuffer
	bufferDesc.ByteWidth = sizeof(LightBuffer);
	result = myDevice->CreateBuffer(&bufferDesc, nullptr, &myLightBuffer);
	if (FAILED(result))
		return false;

	//SettingsBuffer
	bufferDesc.ByteWidth = sizeof(SettingsBuffer);
	result = myDevice->CreateBuffer(&bufferDesc, nullptr, &mySettingsBuffer);
	if (FAILED(result))
		return false;

	return true;
}

bool GraphicsEngine::CreateSamplerState()
{
	HRESULT result;

	D3D11_SAMPLER_DESC sampleDesc = {};
	sampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.MipLODBias = 0.0f;
	sampleDesc.MaxAnisotropy = 8.0f;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampleDesc.MinLOD = 0.0f;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = myDevice->CreateSamplerState(&sampleDesc, &mySampleState);
	if (FAILED(result))
		return false;

	myContext->PSSetSamplers(0, 1, &mySampleState);

	return true;
}

bool GraphicsEngine::CreateWaterRenderTarget(const float aWidth, const float aHeight)
{
	HRESULT result;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = aWidth;
	desc.Height = aHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* texture = nullptr;
	result = myDevice->CreateTexture2D(&desc, nullptr, &texture);
	if (FAILED(result))
		return false;

	result = myDevice->CreateShaderResourceView(texture, nullptr, &myWaterReflectionRenderTarget.myShaderResource);
	if (FAILED(result))
		return false;

	result = myDevice->CreateRenderTargetView(texture, nullptr, &myWaterReflectionRenderTarget.myRenderTargetView);

	texture->Release();

	myContext->PSSetShaderResources(10, 1, &myWaterReflectionRenderTarget.myShaderResource);

	return true;
}

bool GraphicsEngine::CreateFFCRasterizer()
{
	HRESULT result;

	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	rasterDesc.FillMode = D3D11_FILL_SOLID;

	result = myDevice->CreateRasterizerState(&rasterDesc, &myFFCRasterState);
	if (FAILED(result))
		return false;

	return true;
}

bool GraphicsEngine::GeneratePlane(const float aSize, const float aHeightPosition)
{
	CU::Vector3f position;
	CU::Vector2f uv;
	CU::Vector3f normal;
	CU::Vector3f tangent;
	CU::Vector3f bitangent;
	Vertex vertices[]
	{
		CU::Vector3f{0.0f * aSize, aHeightPosition, 0.0f * aSize}, CU::Vector2f{0.0f, 0.0f}, CU::Vector3f{0.0f, 1.0f, 0.0f}, CU::Vector3f{0.0f, 0.0f, 1.0f}, CU::Vector3f{1.0f, 0.0f, 0.0f},
		CU::Vector3f{0.0f * aSize, aHeightPosition, 1.0f * aSize}, CU::Vector2f{0.0f, 1.0f}, CU::Vector3f{0.0f, 1.0f, 0.0f}, CU::Vector3f{0.0f, 0.0f, 1.0f}, CU::Vector3f{1.0f, 0.0f, 0.0f},
		CU::Vector3f{1.0f * aSize, aHeightPosition, 0.0f * aSize}, CU::Vector2f{1.0f, 0.0f}, CU::Vector3f{0.0f, 1.0f, 0.0f}, CU::Vector3f{0.0f, 0.0f, 1.0f}, CU::Vector3f{1.0f, 0.0f, 0.0f},
		CU::Vector3f{1.0f * aSize, aHeightPosition, 1.0f * aSize}, CU::Vector2f{1.0f, 1.0f}, CU::Vector3f{0.0f, 1.0f, 0.0f}, CU::Vector3f{0.0f, 0.0f, 1.0f}, CU::Vector3f{1.0f, 0.0f, 0.0f},
	};

	unsigned int indices[]
	{
		0, 1, 3,
		3, 2, 0
	};

	myWaterPlane = new Primitive(myDevice.Get(), myContext.Get());
	myWaterPlane->Init(vertices, 4, indices, std::size(indices));
	myWaterPlane->SetPosition({ 0.0f, 0.0f, 0.0f });
	myWaterPlane->SetVertexShader(L"ColorVS");
	myWaterPlane->SetPixelShader(L"WaterPS");

	myWaterPlane->AddTexture(myWaterReflectionRenderTarget.myShaderResource, 10);

	return true;
}

bool GraphicsEngine::GenerateTerrain()
{
	bool result;

	std::vector<float> heightMap;

	float resolution = 16.0f;
	float amplitude = 1.2f;

	heightMap.resize(resolution * resolution);
	for (size_t i = 0; i < 4; i++)
	{
		AddNoise(heightMap, amplitude);
		amplitude *= 0.25f;

		heightMap = Upsample2X(heightMap, resolution);
		resolution *= 2.0f;
	}

	Vertex* vertices = new Vertex[heightMap.size()];
	GenerateVertices(vertices, heightMap, resolution);
	GenerateNormals(vertices, heightMap.size(), resolution);

	unsigned int indexCount = ((resolution - 1) * (resolution - 1)) * 6;
	unsigned int* indices = new unsigned int[indexCount];
	GenerateIndices(indices, indexCount, resolution);

	//Textures
	const wchar_t* texture[]
	{
		L"bin/data/textures/Grass_c.png",
		L"bin/data/textures/Rock_c.png",
		L"bin/data/textures/Snow_c.png",
			  
		L"bin/data/textures/Grass_n.png",
		L"bin/data/textures/Rock_n.png",
		L"bin/data/textures/Snow_n.png",
		  
		L"bin/data/textures/Grass_m.png",
		L"bin/data/textures/Rock_m.png",
		L"bin/data/textures/Snow_m.png",
	};

	myTerrain = new Primitive(myDevice.Get(), myContext.Get());
	myTerrain->Init(vertices, heightMap.size(), indices, indexCount, texture, std::size(texture));
	myTerrain->SetPosition({ 0.0f, 0.0f, 0.0f });
	myTerrain->SetVertexShader(L"ColorVS");
	myTerrain->SetPixelShader(L"ColorPS");

	delete[] vertices;
	delete[] indices;
	vertices = nullptr;
	indices  = nullptr;

	return true;
}

void GraphicsEngine::GenerateVertices(Vertex* outVertices, const std::vector<float>& aHeightMap, const float aResolution)
{
	float distanceScalar = 0.08f;
	float uvScalar = 20.0f;

	for (size_t i = 0; i < aHeightMap.size(); i++)
	{
		size_t row    = i / (int)aResolution;
		size_t column = i % (int)aResolution;

		//Position
		outVertices[i].position.x = (float)row    * distanceScalar;
		outVertices[i].position.y = aHeightMap[i];
		outVertices[i].position.z = (float)column * distanceScalar;

		//UV
		outVertices[i].uv.x = row    * (1 / aResolution) * uvScalar;
		outVertices[i].uv.y = column * (1 / aResolution) * uvScalar;
	}
}

void GraphicsEngine::GenerateNormals(Vertex* outVertices, const size_t amountVertices, const float aResolution)
{
	for (size_t i = 0; i < amountVertices; i++)
	{
		const size_t row    = i / (int)aResolution;
		const size_t column = i % (int)aResolution;

		if (row == aResolution - 1 || column == aResolution - 1)
			continue;
		if (row == 0 || column == 0)
			continue;

		const CU::Vector3f above = outVertices[i - (int)aResolution].position;
		const CU::Vector3f below = outVertices[i + (int)aResolution].position;
		const CU::Vector3f right = outVertices[i + 1].position;
		const CU::Vector3f left  = outVertices[i - 1].position;

		const CU::Vector3f vertical = below - above;
		const CU::Vector3f horizontal = right - left;

		const CU::Vector3f Ncross = (horizontal.Cross(vertical)).GetNormalized();
		memcpy(&outVertices[i].normal, &Ncross, sizeof(CU::Vector3f));
		
		
		const CU::Vector3f crossTanget = outVertices[i].normal.Cross(CU::Vector3f(0.0f, 0.0f, 1.0f)).GetNormalized();
		const CU::Vector3f crossBitanget = outVertices[i].normal.Cross(CU::Vector3f(1.0f, 0.0f, 0.0f)).GetNormalized();
		memcpy(&outVertices[i].tangent, &crossTanget, sizeof(CU::Vector3f));
		memcpy(&outVertices[i].bitangent , &crossBitanget, sizeof(CU::Vector3f));
	}
}

void GraphicsEngine::GenerateIndices(unsigned int* outIndices, const size_t indexCount, const float aResolution)
{
	int counter = 0;
	for (size_t i = 0; i < indexCount / 6; i++)
	{
		size_t row    = i / (int)aResolution;
		size_t column = i % (int)aResolution;

		if (row == aResolution - 1 || column == aResolution - 1)
			continue;

		size_t bottomLeft = (row + 1) * aResolution + column;
		size_t bottomRight = (row + 1) * aResolution + (column + 1);
		size_t topLeft = row * aResolution + column;
		size_t topRight = row * aResolution + (column + 1);

		outIndices[counter] = bottomLeft;
		counter++;
		outIndices[counter] = topLeft;
		counter++;
		outIndices[counter] = topRight;
		counter++;
		outIndices[counter] = topRight;
		counter++;
		outIndices[counter] = bottomRight;
		counter++;
		outIndices[counter] = bottomLeft;
		counter++;
	}
}

void GraphicsEngine::UpdateAndBindBuffers()
{
	D3D11_MAPPED_SUBRESOURCE mappedCBuffer = {};
	
	//Frame Buffer
	{
		FrameBuffer frameBufferData = {};
		auto worldToClip = myCamera.GetViewMatrix() * myCamera.GetProjectionMatrix();

		frameBufferData.worldToClipMatrix = worldToClip;
		frameBufferData.totalTime = myTimer.GetTotalTime();
		frameBufferData.cameraPos = myCamera.GetTransform().GetPosition();

		myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCBuffer);
		memcpy(mappedCBuffer.pData, &frameBufferData, sizeof(FrameBuffer));
		myContext->Unmap(myFrameBuffer, 0);
		myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
		myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);
	}

	ZeroMemory(&mappedCBuffer, sizeof(D3D11_MAPPED_SUBRESOURCE));
	//LightBuffer
	{
		LightBuffer lightBufferData = {};

		//DirL
		const auto& dirL = myLight.GetDirectionalLight();
		lightBufferData.directionalLightDir = dirL.myDirection;
		lightBufferData.directionalLightColor = dirL.myColor;

		//AmbientL
		const auto& ambientL = myLight.GetAmbientLight();
		lightBufferData.skyColor = ambientL.mySkyColor;
		lightBufferData.groundColor = ambientL.myGroundColor;

		int size = sizeof(LightBuffer);

		myContext->Map(myLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCBuffer);
		memcpy(mappedCBuffer.pData, &lightBufferData, sizeof(LightBuffer));
		myContext->Unmap(myLightBuffer, 0);
		myContext->PSSetConstantBuffers(2, 1, &myLightBuffer);
	}
}

void GraphicsEngine::Render()
{
	//Render to texture upside down
	RenderWaterToTexture();

	SetRenderTargets(myBackBuffer.GetAddressOf(), nullptr);
	UpdateAndBindBuffers();
	RenderPrimitive(myTerrain);

	//Render Water
	RenderPrimitive(myWaterPlane);

	mySwapChain->Present(1, 0);
}

void GraphicsEngine::RenderWaterToTexture()
{
	SetRenderTargets(&myWaterReflectionRenderTarget.myRenderTargetView, myFFCRasterState);

	D3D11_MAPPED_SUBRESOURCE mappedCBuffer = {};

	//EXTRA LEFT HERE!!!
	//Frame Buffer
	{
		FrameBuffer frameBufferData = {};
		auto worldToClip = myCamera.GetReflectionMatrix(myWaterHeight);

		frameBufferData.worldToClipMatrix = worldToClip;
		frameBufferData.totalTime = myTimer.GetTotalTime();
		frameBufferData.cameraPos = myCamera.GetTransform().GetPosition();

		myContext->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCBuffer);
		memcpy(mappedCBuffer.pData, &frameBufferData, sizeof(FrameBuffer));
		myContext->Unmap(myFrameBuffer, 0);
		myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
		myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);
	}

	UpdateAndBindBuffers();
	RenderPrimitive(myTerrain);
}

void GraphicsEngine::SetRenderTargets(ID3D11RenderTargetView** aRenderTarget, ID3D11RasterizerState* aRasterState)
{
	constexpr float color[4] = { 0.97f, 0.59f, 0.38f, 1.0f };

	myContext->OMSetRenderTargets(1, aRenderTarget, myDepthBuffer);
	myContext->ClearRenderTargetView(*aRenderTarget, color);
	myContext->ClearDepthStencilView(myDepthBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	myContext->RSSetState(aRasterState);
}

void GraphicsEngine::RenderPrimitive(Primitive* aPrimitive)
{
	D3D11_MAPPED_SUBRESOURCE mappedCBuffer = {};

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
	float t = myTimer.GetTotalTime();
	float rotSpeed = 0.3f;
	myLight.SetDirectionalLight({std::sin(t * rotSpeed), std::cos(t * rotSpeed), 0.0f}, {1.0f, 1.0f, 1.0, 1.0f});
	//myLight.SetAmbientLight({ 1.0f - CU::Clamp(0.1f, 0.8f, CU::Clamp(0.0f, 0.8f, std::sin(t * rotSpeed))), 0.25f, 0.35, 1.0f }, { 0.65f, 0.5f, 0.37f, 1.0f });

	myTimer.Update();

	myCamera.Update(myTimer.GetDeltaTime());
}