#pragma once
#include <wrl/client.h>
#include "CU/Timer.h"
#include "Camera.h"

using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Texture2D;
struct ID3D11Buffer;

class Primitive;

	class GraphicsEngine
	{
		//Added
		struct FrameBuffer
		{
			CU::Matrix4x4f worldToClipMatrix;
			float totalTime;
			float padding[3];
		};

		//Added
		struct ObjectBuffer
		{
			CU::Matrix4x4f objectMatrix;
		};

	public:
		GraphicsEngine();
		~GraphicsEngine();

		bool Init(int aHeight, int aWidth, HWND aWindowHandle);
		void Render();
		void Update();
	private:
		//ADDED
		void RenderPrimitive(Primitive* aPrimitive);

		ComPtr<ID3D11Device>	       myDevice;
		ComPtr<ID3D11DeviceContext>    myContext;
		ComPtr<IDXGISwapChain>         mySwapChain;
		ComPtr<ID3D11RenderTargetView> myRenderTarget;

		//ADDED
		ComPtr<ID3D11DepthStencilView> myDepthStencilView;
		ComPtr<ID3D11Texture2D>		   myDepthBuffer;

		//ADDED
		ID3D11Buffer*				   myFrameBuffer;
		ID3D11Buffer*                  myObjectBuffer;

		//ADDED
		Primitive* myCube;
		Primitive* myPyramid;
		Camera	   myCamera;

		CU::Timer myTimer;
	};