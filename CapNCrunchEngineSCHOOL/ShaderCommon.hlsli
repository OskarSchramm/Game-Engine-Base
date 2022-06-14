Texture2D grassTexture : register(t0);
Texture2D rockTexture : register(t1);
Texture2D snowTexture : register(t2);

Texture2D grassNormal : register(t3);
Texture2D rockNormal : register(t4);
Texture2D snowNormal : register(t5);

SamplerState sampleState : register(s0);

cbuffer FrameBuffer : register(b0)
{
    float4x4 modelToClipMatrix;
    float totalTime;
    float3 padding;
};

cbuffer ObjectBuffer : register(b1)
{
    float4x4 modelMatrix;
};

cbuffer LightBuffer : register(b2)
{
    float4 directionalLightColor;
    float4 skyColor;
    float4 groundColor;
    float3 directionalLightDir;
    float  padding0;
}

struct VertexInputType
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float3 worldPosition : WORLDPOSITION;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};

float3 expandNormal(float4 normalTexture)
{
    float3 normal = normalTexture.agg;
    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1 - saturate((normal.x * normal.x) + (normal.y * normal.y)));
    return normalize(normal);
}