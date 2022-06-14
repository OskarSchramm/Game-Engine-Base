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
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};