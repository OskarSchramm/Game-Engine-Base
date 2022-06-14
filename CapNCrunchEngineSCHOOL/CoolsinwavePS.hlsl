struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float totalTime : TIME;
};
struct PixelOutput
{
    float4 color : SV_TARGET;
};
PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    result.color = input.color;
    result.color.y = sin(dot(input.position, input.color * 0.5) * sin(input.totalTime));
    return result;
}