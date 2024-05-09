//ADDED
cbuffer FrameBuffer : register(b0)
{
    float4x4 modelToClipMatrix;
    float totalTime;
    float3 padding;
};

//ADDED
cbuffer ObjectBuffer : register(b1)
{
    float4x4 modelMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float totalTime : TIME;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
    //ADDED
    float4 worldPos = mul(modelMatrix, input.position);
    float4 vertexToClipPos = mul(modelToClipMatrix, worldPos);
    output.position = vertexToClipPos;
    
    //fun little test
    output.totalTime = totalTime;
    output.color = input.color;
    output.color.r = clamp(sin(totalTime), 0.2, 1) * input.color.r;
    
    return output;
}