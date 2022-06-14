struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float totalTime : TIME;
    float4 worldPos : WORLDPOS;
};
struct PixelOutput
{
    float4 color : SV_TARGET;
};


Texture2D aTexture : register(t0);
SamplerState aSampleState : register(s0);

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    result.color = input.color;
    result.color = aTexture.Sample(aSampleState, input.uv.xy).rgba;
    result.color.r = sin(length(input.worldPos) * 2.f * 3.14f + input.totalTime); 
    return result;
}