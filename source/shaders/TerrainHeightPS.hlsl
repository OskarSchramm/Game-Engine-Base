#include "ShaderStructs.hlsli"
#include "ShaderCommon.hlsli"
#include "CommonFunctions.hlsli"
#include "PBRFunctions.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    
    result.color.r = input.normal.x;
    result.color.g = input.normal.y;
    result.color.b = input.normal.z;
    result.color.a = clamp(input.worldPosition.y, 0, 1);
    
    return result;
}