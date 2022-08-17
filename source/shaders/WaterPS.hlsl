#include "ShaderStructs.hlsli"
#include "ShaderCommon.hlsli"
#include "CommonFunctions.hlsli"
#include "PBRFunctions.hlsli"

Texture2D reflectionTexture : register(t10);

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
        
    float4x4 cameraTranspose = transpose(cameraMatrix);
    float3 toEye = cameraTranspose[3].xyz - input.worldPosition.xyz;
    float dist = abs(dot(toEye, cameraTranspose[2].xyz));
    
    float2 p = input.worldPosition.xz;
    float2 k0 = float2(6.f, 16.f);
    float2 k1 = float2(10.f, -14.f);
    float A = 0.0005f;
    
    float2 heightDerivative = k0 * sin(dot(p, k0) + totalTime) + k1 * sin(dot(p, k1) + totalTime);
    float2 maxValue = float2(0.f, length(k0) + length(k1));
    
    float offset = A * (maxValue + heightDerivative) / dist;
    
    float fresnel = Fresnel_Schlick(float3(0.25f, 0.25f, 0.25f), float3(0.0f, 1.0f, 0.0f), toEye);

    float2 resolution = float2(aResolutionHeight, aResolutionWidth);
    float3 reflection = reflectionTexture.Sample(sampleState, input.position.xy / resolution + offset).rgb;
    
    float3 resCol = tonemap_s_gamut3_cine(fresnel * reflection);
    result.color.rgb = resCol;
    result.color.a = 1.0f;
    
    return result;
}