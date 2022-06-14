#pragma once

enum class TextureType
{
	GrassAlbedo,
	StoneAlbedo,
	SnowAlbedo,

	GrassNormal,
	StoneNormal,
	SnowNormal,
};

struct TexturePaths
{
	const wchar_t* myGrassAlbedo;
	const wchar_t* myStoneAlbedo;
	const wchar_t* mySnowAlbedo;

	const wchar_t* myGrassNormal;
	const wchar_t* myStoneNormal;
	const wchar_t* mySnowNormal;
};

class Texture;
struct TextureData
{
	TextureData(TextureType aSlot) { mySlotType = aSlot; }
	TextureType mySlotType;
	Texture myResource;
};