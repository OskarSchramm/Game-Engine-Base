#include "ShaderCommon.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    
    //TBN Matrix in Object Space
    float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
    
    //Albedo
    float4 sampledGrassA = grassTexture.Sample(sampleState, input.uv.xy).rgba;
    float4 sampledRockA  = rockTexture.Sample(sampleState, input.uv.xy).rgba;
    float4 sampledSnowA  = snowTexture.Sample(sampleState, input.uv.xy).rgba;
    
    //Grass Normal
    float4 sampledGrassN        = grassNormal.Sample(sampleState, input.uv.xy).rgba;
    float3 sampledGrassNormal   = expandNormal(sampledGrassN);
    
    //Rock Normal
    float4 sampledRockN        = rockNormal.Sample(sampleState, input.uv.xy).rgba;
    float3 sampledRockNormal   = expandNormal(sampledRockN);
    
    //Snow Normal
    float4 sampledSnowN        = snowNormal.Sample(sampleState, input.uv.xy).rgba;
    float3 sampledSnowNormal   = expandNormal(sampledSnowN);
    
    //LightColor
    float4 materialColor = directionalLightColor;
    float4 sky    = (0.5f + 0.5f * input.normal.y) * skyColor;
    float4 ground = (0.5f - 0.5f * input.normal.y) * groundColor;
    
    //Lerping terrain
    float slopeBlend  = smoothstep(0.7f, 1.0f, input.normal.y);
    float heightBlend = smoothstep(-0.05f, 0.25f, input.worldPosition.y);
    float3 albedoColor = lerp(sampledRockA, lerp(sampledGrassA, sampledSnowA, heightBlend), slopeBlend).rgb;
    float3 normal = lerp(sampledRockNormal, lerp(sampledGrassNormal, sampledSnowNormal, heightBlend), slopeBlend);
    normal = normalize(mul(normal, TBN));
    
    //Calculate light
    float4 ambient = (sky + ground);
    float4 diffuse = directionalLightColor * max(0.1f, dot(directionalLightDir, normal));
    
    materialColor = float4(albedoColor, 1.0) * (diffuse + ambient);
    result.color = materialColor;
    
    return result;
}