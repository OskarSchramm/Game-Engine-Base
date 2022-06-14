#include "ShaderCommon.hlsli"

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
    float4 pos = float4(input.position, 1.0f);
    float4 worldPos = mul(modelMatrix, pos);
    float4 vertexToClipPos = mul(modelToClipMatrix, worldPos);
    output.position = vertexToClipPos;
    
    float4 vertexObjectNormal = float4(input.normal, 0.0f);
    float4 vertexWorldNormal  = mul(modelMatrix, vertexObjectNormal);
    output.normal = vertexWorldNormal;
    
    output.uv = input.uv;
    
    return output;
}