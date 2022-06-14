struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
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
    
    result.color = aTexture.Sample(aSampleState, input.uv.xy).rgba;
    
    return result;
}