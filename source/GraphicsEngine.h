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
#include "RenderTarget.h"

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
		CU::Matrix4x4f cameraMatrix;
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

	struct SettingsBuffer
	{
		float myResolutionWidth;
		float myResolutionHeight;
		float myClipPlaneHeight;
		float padding;
	};

public:
	GraphicsEngine();
	~GraphicsEngine();

	bool Init(int aWidth, int aHeight, HWND aWindowHandle);
	void Render();
	void Update();
private:
	bool CreateDeviceAndSwapChain(HWND* aWindowHandle);
	bool CreateViewPort(int& aWidth, int& aHeight);
	bool CreateDepthBufferAndStencil(const UINT aWidth, const UINT aHeight);
	bool CreateConstantBuffers();
	bool CreateSamplerState();
	void UpdateAndBindBuffers();

	//ADDED
	bool CreateWaterRenderTarget(const float aWidth, const float aHeight);
	bool CreateFFCRasterizer();
	bool GeneratePlane(const float aSize, const float aHeightPosition);
	void UpdateSettingsBuffer(const float aWidth, const float aHeight);
	void RenderWaterToTexture();
	void SetRenderTargets(ID3D11RenderTargetView** aRenderTarget, ID3D11RasterizerState* aRasterState);

	bool GenerateTerrain();
	void GenerateVertices(Vertex* outVertices, const std::vector<float>& aHeightMap, const float aResolution);
	void GenerateNormals(Vertex* outVertices, const size_t amountVertices, const float aResolution);
	void GenerateIndices(unsigned int* outIndices, const size_t indexCount, const float aResolution);
	
	void RenderPrimitive(Primitive* aPrimitive);

	ComPtr<ID3D11Device>	       myDevice;
	ComPtr<ID3D11DeviceContext>    myContext;
	ComPtr<IDXGISwapChain>         mySwapChain;

	ComPtr<ID3D11RenderTargetView> myBackBuffer;
	ID3D11DepthStencilView*		   myDepthBuffer;

	ID3D11SamplerState*            mySampleState;

	ID3D11Buffer*				   myFrameBuffer;
	ID3D11Buffer*                  myObjectBuffer;
	ID3D11Buffer*			       myLightBuffer;
	ID3D11Buffer*                  mySettingsBuffer;

	SettingsBuffer				   mySettingsData;

	const float myWaterHeight = 0.0f; //-0.25
	Primitive* myWaterPlane;
	RenderTarget myWaterReflectionRenderTarget;
	ID3D11RasterizerState* myFFCRasterState;
	
	Texture    myCubeMap;
	Light	   myLight;

	Camera	   myCamera;
	CU::Timer  myTimer;

	Primitive* myTerrain;
};