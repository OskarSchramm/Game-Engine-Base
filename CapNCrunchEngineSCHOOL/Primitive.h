#pragma once
#include <Windows.h>
#include <minwindef.h>
#include "CU/Matrix4x4.hpp"
#include "Texture.h"
#include "TextureTypes.h"
#include <vector>

struct Vertex;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

class Primitive
{
public:
	Primitive(ID3D11Device* aDevice, ID3D11DeviceContext* aDeviceContext);
	~Primitive();

	bool Init(Vertex* vertices, UINT aVertexCount, UINT* indices, UINT aIndexCount, const wchar_t** someTexturePaths, UINT aTextureSize);
	void Render();

	void SetPosition(const CU::Vector3f& aPos);
	bool SetVertexShader(const LPCWSTR aShaderFilename);
	bool SetPixelShader(const LPCWSTR aShaderFilename);

	CU::Matrix4x4f GetModelMatrix() { return myModelMatrix; }
private:
	void SetIAStuff();
	void Shutdown();

	UINT myIndexCount;

	ID3D11Device* myDevicePtr;
	ID3D11DeviceContext* myDeviceContextPtr;

	ID3D11Buffer* myVertexBuffer;
	ID3D11Buffer* myIndexBuffer;
	ID3D11VertexShader* myVertexShader;
	ID3D11PixelShader* myPixelShader; 
	ID3D11InputLayout* myInputLayout;

	std::vector<TextureData> myTextures;

	CU::Matrix4x4f myModelMatrix;
};