#include "ShaderStructs.hlsli"
#include "ShaderCommon.hlsli"
#include "CommonFunctions.hlsli"
#include "PBRFunctions.hlsli"

Texture2D reflected : register(t10);

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    
    float2 resolution = float2(aResolutionWidth, aResolutionHeight);
    float2 offset = A * (maxValue + heightDerivative) / distance;
    float3 reflection = reflection.Sample(sampleState, input.position.xy / resolution + offset).rgb;
    
    return result;
}