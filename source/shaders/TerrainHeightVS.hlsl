#include "ShaderStructs.hlsli"
#include "ShaderCommon.hlsli"

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
    //MVP
    float4 pos = float4(input.position, 1.0f);
    float4 worldPos = mul(modelMatrix, pos);
    
    //Normal
    float4 vertexObjectNormal = float4(input.normal, 0.0f);
    float4 vertexWorldNormal = mul(modelMatrix, vertexObjectNormal);
    output.normal = normalize(vertexWorldNormal);
    
    //Tangent
    float4 vertexObjectTangent = float4(input.tangent, 0.0f);
    float4 vertexWorldTangent = mul(modelMatrix, vertexObjectTangent);
    output.tangent = normalize(vertexWorldTangent);
    
    //Bitangent
    float4 vertexObjectBitangent = float4(input.bitangent, 0.0f);
    float4 vertexWorldBitangent = mul(modelMatrix, vertexObjectBitangent);
    output.bitangent = normalize(vertexWorldBitangent);
    
    output.position = float4(input.lmCoord.x, input.lmCoord.y, 0, 0);
    output.worldPosition = worldPos;
    output.uv = input.uv;
    output.lmCoord = input.lmCoord;
    output.clip = 0.0f;
    
    return output;
}