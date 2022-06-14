#include "Texture.h"

#include "stb_image.h"

#include <string>
#include <vector>

Texture::Texture() = default;
Texture::~Texture() = default;

bool Texture::Init(ID3D11Device* aDevice, const wchar_t* aTextureName, bool useStbImage)
{
	return (useStbImage) ? stb_load(aDevice, aTextureName) : dx_load(aDevice, aTextureName);
}

bool Texture::dx_load(ID3D11Device* aDevice, const wchar_t* aTextureName)
{
	HRESULT result = DirectX::CreateDDSTextureFromFile(aDevice, aTextureName, nullptr, &myResourceView);
	if (FAILED(result))
		return false;

	return true;
}

bool Texture::stb_load(ID3D11Device* aDevice, const wchar_t* aTextureName)
{
	int width, height, channels;
	std::string sTexturePath(std::wstring(aTextureName));
	//const char* ccTexturePath = sTexturePath.c_str();

	unsigned char* img = stbi_load("Temp", &width, &height, &channels, 0);
	if (img == nullptr)
		return false;

	if (channels == 3)
	{
		std::vector<unsigned char> imageData(width * height * 4);
		for (int i = 0; i < width * height; i++)
		{
			imageData[4 * i + 0] = img[3 * i + 0];
			imageData[4 * i + 1] = img[3 * i + 1];
			imageData[4 * i + 2] = img[3 * i + 2];
			imageData[4 * i + 3] = 255;
		}
		if (!stb_init(aDevice, imageData.data(), width, height))
			return false;
	}
	else if (channels == 4)
	{
		if (!stb_init(aDevice, img, width, height))
			return false;
	}
	else
	{
		return false;
	}
}

bool Texture::stb_init(ID3D11Device* device, unsigned char* rgbaPixels, int width, int height)
{
	ID3D11Texture2D* texture = nullptr;
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subresourceDesc = {};
	subresourceDesc.pSysMem = (void*)rgbaPixels;
	subresourceDesc.SysMemPitch = width * 4;
	subresourceDesc.SysMemSlicePitch = width * height * 4;
	if (FAILED(device->CreateTexture2D(&desc, &subresourceDesc, &texture)))
		return false;
	HRESULT hr = device->CreateShaderResourceView(texture, NULL, &myResourceView);
	if (FAILED(hr))
	{
		return false;
	}
	texture->Release();
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