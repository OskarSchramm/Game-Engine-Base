#pragma once
#pragma comment( lib, "dxguid.lib")
#include <DDSTextureLoader/DDSTextureLoader11.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

class Texture
{
public:
	Texture();
	~Texture();

	bool Init(ID3D11Device* aDevice, const wchar_t* aTextureName);
	void Bind(ID3D11DeviceContext* aContext, const int aSlot);

	void Release();
private:
	ID3D11ShaderResourceView* myResourceView;
};

