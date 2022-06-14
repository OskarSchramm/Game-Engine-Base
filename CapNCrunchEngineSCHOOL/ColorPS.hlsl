struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};
struct PixelOutput
{
    float4 color : SV_TARGET;
};
PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    result.color = input.color;
    return result;
}