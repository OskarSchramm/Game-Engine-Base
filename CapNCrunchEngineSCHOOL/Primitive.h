#pragma once
#include <Windows.h>
#include <minwindef.h>

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

	bool Init(Vertex* vertices, UINT aVertexCount, UINT* indices, UINT aIndexCount);
	void Render();

	bool SetVertexShader(const LPCWSTR aShaderFilename);
	bool SetPixelShader(const LPCWSTR aShaderFilename);
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
};