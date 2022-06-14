#pragma once
#pragma comment( lib, "dxguid.lib")
#include <DDSTextureLoader/DDSTextureLoader11.h>
#include "CU/Vector2.hpp"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

class Texture
{
public:
	Texture();
	~Texture();

	bool Init(ID3D11Device* aDevice, const wchar_t* aTextureName, bool useStbImage = false);
	void Bind(ID3D11DeviceContext* aContext, const int aSlot);

	void Release();
private:
	bool dx_load(ID3D11Device* aDevice, const wchar_t* aTextureName);

	bool stb_load(ID3D11Device* aDevice, const wchar_t* aTextureName);
	bool stb_init(ID3D11Device* device, unsigned char* rgbaPixels, int width, int height);

	ID3D11ShaderResourceView* myResourceView;
};