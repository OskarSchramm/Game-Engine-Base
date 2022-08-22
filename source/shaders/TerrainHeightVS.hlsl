#include "ShaderStructs.hlsli"
#include "ShaderCommon.hlsli"

LightmapInputType main(VertexInputType input)
{
    LightmapInputType output;
    
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
    
    float lmX = input.lmCoord.x - 1.0f;
    float lmY = input.lmCoord.y - 1.0f;
    float2 lmPos = float2((lmX), (lmY));
    
    output.position = float4(lmPos.x, lmPos.y, 0.0f, 1.0f);
    output.worldPosition = worldPos;
    output.lmCoord = input.lmCoord;
    
    return output;
}