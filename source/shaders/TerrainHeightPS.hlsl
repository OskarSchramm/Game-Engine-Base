#include "ShaderStructs.hlsli"
#include "ShaderCommon.hlsli"
#include "CommonFunctions.hlsli"
#include "PBRFunctions.hlsli"

PixelOutput main(LightmapInputType input)
{
    PixelOutput result;
    
    result.color.r = input.normal.x;
    result.color.g = input.normal.y;
    result.color.b = input.normal.z;
    result.color.a = clamp((1 + input.worldPosition.y) / 2, 0, 1);

    return result;
}