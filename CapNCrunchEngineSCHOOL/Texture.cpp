#include "Texture.h"

Texture::Texture() = default;
Texture::~Texture() = default;

bool Texture::Init(ID3D11Device* aDevice, const wchar_t* aTextureName)
{
	myResourceView = nullptr;

	HRESULT result = DirectX::CreateDDSTextureFromFile(aDevice, aTextureName, nullptr, &myResourceView);
	if (FAILED(result))
		return false;

	return true;
}

void Texture::Bind(ID3D11DeviceContext* aContext, const int aSlot)
{
	aContext->PSSetShaderResources(aSlot, 1, &myResourceView);
}

void Texture::Release()
{
	if (myResourceView)
	{
		myResourceView->Release();
		myResourceView = nullptr;
	}
}