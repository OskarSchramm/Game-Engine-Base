#pragma once

enum class TextureType
{
	GrassAlbedo,
	StoneAlbedo,
	SnowAlbedo,

	GrassNormal,
	StoneNormal,
	SnowNormal,

	//ADDED
	GrassMaterial,
	StoneMaterial,
	SnowMaterial,

	CubeMap
};

struct ID3D11ShaderResourceView;

class Texture;
struct TextureData
{
	TextureData(const int aSlot)
	{
		mySlotType = (TextureType)aSlot;
	}

	TextureData(ID3D11ShaderResourceView* aShaderResource, const int aSlot)
	{ 
		mySlotType = (TextureType)aSlot;
		myResource.myResourceView = aShaderResource;
	}

	TextureType mySlotType;
	Texture myResource;
};