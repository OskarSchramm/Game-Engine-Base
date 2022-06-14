#pragma once
#pragma comment( lib, "dxguid.lib")
#include <DDSTextureLoader/DDSTextureLoader11.h>
#include "CU/Vector2.hpp"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

enum class TextureType;

class Texture
{
public:
	Texture();
	~Texture();

	//useSrgb true for default textures, false for detailed textures, specific use cases such as normals
	bool Init(ID3D11Device* aDevice, ID3D11DeviceContext* aContext, const wchar_t* aTextureName, bool useStbImage = false, bool useSrgb = true, bool generateMipMap = false);
	void Bind(ID3D11DeviceContext* aContext, TextureType* aType);

	void Release();

	inline bool IsLoaded() { return myResourceView ? true : false; }
private:
	bool dx_load(ID3D11Device* aDevice, const wchar_t* aTextureName);

	bool stb_load(ID3D11Device* aDevice, ID3D11DeviceContext* aContext, const wchar_t* aTextureName);
	bool stb_init(ID3D11Device* device, ID3D11DeviceContext* aContext, unsigned char* rgbaPixels, int width, int height);

	bool myIsSrgb = true;
	bool myGenMipMap = false;
	ID3D11ShaderResourceView* myResourceView;
};