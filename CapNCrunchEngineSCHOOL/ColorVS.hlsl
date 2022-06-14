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

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float totalTime : TIME;
    float4 worldPos : WORLDPOS;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
    float4 worldPos = mul(modelMatrix, input.position);
    float4 vertexToClipPos = mul(modelToClipMatrix, worldPos);

    output.worldPos = worldPos;
    output.position = vertexToClipPos;
    
    output.totalTime = totalTime;
    output.color = input.color;
    output.uv = input.uv;
    
    return output;
}