#pragma once

//stdlib
#include <vector>

//microsoft dx11
#include <wrl/client.h>

//cpc cu
#include "CU/Timer.h"

//cpc
#include "Camera.h"
#include "Light.h"
#include "Texture.h"

using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Texture2D;
struct ID3D11Buffer;
struct ID3D11SamplerState;
struct ID3D11DepthStencilState;

struct Vertex;
class Primitive;


class GraphicsEngine
{
	struct FrameBuffer
	{
		CU::Matrix4x4f worldToClipMatrix;
		CU::Vector3f cameraPos;
		float totalTime;
	};
	struct ObjectBuffer
	{
		CU::Matrix4x4f objectMatrix;
	};
	struct LightBuffer
	{
		CU::Vector4f directionalLightColor;
		CU::Vector4f skyColor;
		CU::Vector4f groundColor;
		CU::Vector3f directionalLightDir;
		float padding;
	};

public:
	GraphicsEngine();
	~GraphicsEngine();

	bool Init(int aWidth, int aHeight, HWND aWindowHandle);
	void Render();
	void Update();
private:
	//ADDED
	bool CreateDeviceAndSwapChain(HWND* aWindowHandle);
	bool CreateViewPort(int& aWidth, int& aHeight);
	bool CreateDepthBufferAndStencil(ID3D11DepthStencilState* anOutDepthStencilState, const UINT aWidth, const UINT aHeight);
	bool CreateConstantBuffers();
	bool CreateSamplerState();
	void UpdateAndBindBuffers();

	//Terrain
	bool GenerateTerrain();
	void GenerateVertices(Vertex* outVertices, const std::vector<float>& aHeightMap, const float aResolution);
	void GenerateNormals(Vertex* outVertices, const size_t amountVertices, const float aResolution);
	void GenerateIndices(unsigned int* outIndices, const size_t indexCount, const float aResolution);

	void RenderPrimitive(Primitive* aPrimitive);

	ComPtr<ID3D11Device>	       myDevice;
	ComPtr<ID3D11DeviceContext>    myContext;
	ComPtr<IDXGISwapChain>         mySwapChain;
	ComPtr<ID3D11RenderTargetView> myRenderTarget;
	ComPtr<ID3D11DepthStencilView> myDepthStencilView;
	ComPtr<ID3D11Texture2D>		   myDepthBuffer;
	ID3D11SamplerState*            mySampleState;

	ID3D11Buffer*				   myFrameBuffer;
	ID3D11Buffer*                  myObjectBuffer;

	//ADDED
	ID3D11Buffer*                  myLightBuffer;

	//CHANGED/ADDED
	Texture    myCubeMap;

	Primitive* myTerrain; 
	Light	   myLight;

	Camera	   myCamera;
	CU::Timer  myTimer;
};