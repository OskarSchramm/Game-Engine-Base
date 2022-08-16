#include "ShaderStructs.hlsli"
#include "ShaderCommon.hlsli"
#include "CommonFunctions.hlsli"
#include "PBRFunctions.hlsli"

Texture2D reflectionTexture : register(t10);

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    
    float3 toEye = normalize(cameraPosition.xyz - input.worldPosition.xyz);
    
    float fresnel = Fresnel_Schlick(float3(0.25f, 0.25f, 0.25f), float3(0.0f, 1.0f, 0.0f), toEye);

    float2 resolution = float2(aResolutionWidth, aResolutionHeight);
    float3 reflection = reflectionTexture.Sample(sampleState, input.position.xy / resolution).rgb;
    
    result.color.rgb = fresnel * reflection;
    result.color.a = 1.0f;
    
    return result;
}