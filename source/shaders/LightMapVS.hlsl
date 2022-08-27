#include "ShaderStructs.hlsli"
#include "ShaderCommon.hlsli"

LightmapInputType main(VertexInputType input)
{
    LightmapInputType output;
    
    float4 pos = float4(input.position, 1.0f);
    float4 worldPos = mul(modelMatrix, pos);
    
    float lmX = input.lmCoord.x;
    float lmY = input.lmCoord.y;
    float2 lmPos = float2((lmX), (lmY));
    output.position = float4(lmPos.x, lmPos.y, 0.0f, 1.0f);
    output.worldPosition = worldPos;
    output.lmCoord = float2(input.lmCoord.x, -input.lmCoord.y);
    
    return output;
}