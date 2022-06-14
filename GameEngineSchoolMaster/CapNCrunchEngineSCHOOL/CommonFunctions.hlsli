int GetNumMips(TextureCube cubeTex)
{
    int iWidth = 0;
    int iheight = 0;
    int numMips = 0;
    cubeTex.GetDimensions(0, iWidth, iheight, numMips);
    return numMips;
}

float3 expandNormal(float4 normalTexture)
{
    float3 normal = normalTexture.agg;
    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1 - saturate((normal.x * normal.x) + (normal.y * normal.y)));
    return normalize(normal);
}

float3 normal_blend_unity(float4 n1, float4 n2, float aValue)
{
    float scalar = aValue * 4;
    n1 = n1.xyzz * float4(scalar, scalar, scalar, -scalar) + float4(-1, -1, -1, 1);
    n2 = n2 * scalar - 1;
    float3 r;
    r.x = dot(n1.zxx, n2.xyz);
    r.y = dot(n1.yzy, n2.xyz);
    r.z = dot(n1.xyw,-n2.xyz);
    return normalize(r);
}

// Origin: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 s_curve(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

float3 tonemap_s_gamut3_cine(float3 c)
{
    // based on Sony's s gamut3 cine
    float3x3 fromSrgb = float3x3(
        +0.6456794776, +0.2591145470, +0.0952059754,
        +0.0875299915, +0.7596995626, +0.1527704459,
        +0.0369574199, +0.1292809048, +0.8337616753);

    float3x3 toSrgb = float3x3(
        +1.6269474099, -0.5401385388, -0.0868088707,
        -0.1785155272, +1.4179409274, -0.2394254004,
        +0.0444361150, -0.1959199662, +1.2403560812);

    return mul(toSrgb, s_curve(mul(fromSrgb, c)));
}