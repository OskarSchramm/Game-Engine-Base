#include "ShaderCommon.hlsli"

Texture2D aTexture : register(t0);
SamplerState aSampleState : register(s0);

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    float4 sampledTextureColor = aTexture.Sample(aSampleState, input.uv.xy).rgba;
    
    float4 materialColor = directionalLightColor;
    float4 sky    = (0.5f + 0.5f * input.normal.y) * skyColor;
    float4 ground = (0.5f - 0.5f * input.normal.y) * groundColor;
    
    float4 ambient = (sky + ground);
    float4 diffuse = directionalLightColor * max(0.1f, dot(directionalLightDir, input.normal.xyz));
    materialColor = materialColor * (diffuse * ambient);
    materialColor = materialColor * sampledTextureColor;
    result.color = materialColor;
    
    return result;
}