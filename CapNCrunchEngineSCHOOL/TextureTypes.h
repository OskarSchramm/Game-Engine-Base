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

struct TexturePaths
{
	const wchar_t* myGrassAlbedo;
	const wchar_t* myStoneAlbedo;
	const wchar_t* mySnowAlbedo;
};

class Texture;
struct TextureData
{
	TextureData(TextureType aSlot) { mySlotType = aSlot; }
	TextureType mySlotType;
	Texture myResource;
};