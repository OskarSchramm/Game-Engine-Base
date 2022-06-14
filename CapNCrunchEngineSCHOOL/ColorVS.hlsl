#include "ShaderCommon.hlsli"

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
    //MVP
    float4 pos = float4(input.position, 1.0f);
    float4 worldPos = mul(modelMatrix, pos);
    float4 vertexToClipPos = mul(modelToClipMatrix, worldPos);
    output.position = vertexToClipPos;
    
    output.worldPosition = worldPos;
    
    //Normal
    float4 vertexObjectNormal = float4(input.normal, 0.0f);
    float4 vertexWorldNormal  = mul(modelMatrix, vertexObjectNormal);
    output.normal = vertexWorldNormal;
    
    //Tangent
    float4 vertexObjectTangent = float4(input.tangent, 0.0f);
    float4 vertexWorldTangent = mul(modelMatrix, vertexObjectTangent);
    output.tangent = vertexWorldTangent;
    
    //Bitangent
    float4 vertexObjectBitangent = float4(input.bitangent, 0.0f);
    float4 vertexWorldBitangent = mul(modelMatrix, vertexObjectBitangent);
    output.bitangent = vertexWorldBitangent;
    
    output.uv = input.uv;
    
    return output;
}