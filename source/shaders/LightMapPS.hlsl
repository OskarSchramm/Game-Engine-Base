#include "ShaderStructs.hlsli"
#include "ShaderCommon.hlsli"
#include "CommonFunctions.hlsli"
#include "PBRFunctions.hlsli"

Texture2D heightTexture : register(t11);
Texture2D noiseTexture : register(t12);

float rand_1_05(in float2 uv)
{
    float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
    return abs(noise.x + noise.y) * 0.5;
}

float3 RandomUnitVector(in float randNoiseNumber)
{
    float z = randNoiseNumber * 2.0f - 1.0f;
    float a = randNoiseNumber * 2.0f * PI;
    float r = sqrt(1.0f - z * z);
    float x = r * cos(a);
    float y = r * sin(a);
    return float3(x, y, z);
}

float2 fibonacci_lattice(int i, int N)
{
    float PHI = 0.5 * (sqrt(5.) + 1.);
    return float2((float(i) + 0.5f) / float(N), fmod(float(i) / PHI, 1.));
}

float3 fibonacci_sphere(int i, int N)
{
    float2 xy = fibonacci_lattice(i, N);
    float2 pt = float2(2.f * PI * xy.y, acos(2.f * xy.x - 1.f) - PI * 0.5f);
    return float3(cos(pt.x) * cos(pt.y), sin(pt.x) * cos(pt.y), sin(pt.y));
}

PixelOutput main(LightmapInputType input)
{
    PixelOutput result;
    
    float2 offsetSample = (1 + input.lmCoord) / 2;
    int mipMapLevel = 0;
    
    //break out variables from texture
    float4 sampledColor = heightTexture.Sample(sampleState, offsetSample).rgba;
    float4 randomNoise = noiseTexture.Sample(sampleState, input.position.xy).rgba;
    float3 normal = float3(sampledColor.x, sampledColor.y, sampledColor.z);
    float height = float(sampledColor.a);
    
    float AOscalar = 1.2f;
    float resultAO = 0.0f;
    float N = 128;   //how many rays
    float RMA = 64; //ray marching amount
    float RML = 0.25;   //ray marching length
    float rayMarchMaxLengt = RMA * RML;
    float3 rayOrigin = float3(input.worldPosition.x, height, input.worldPosition.z);
    
    //Ray Marching AO
    [fastopt]
    for (int i = 0; i < N; i++)
    {
        //float randNumber = rand_1_05(float2(randomNoise.x, randomNoise.y));
        //float3 randomRayDir = float3(normalize(normal + RandomUnitVector(randNumber)));
        float3 randomRayDir = fibonacci_sphere(i, N);
        
        float d0 = 0; //How far marched from origin;
        [fastopt]
        for (int k = 0; k < RMA; k++)
        {
            d0 += RML;
            float3 AOray = rayOrigin + normalize(randomRayDir) * d0;
            
            float rayL = sqrt(AOray.x * AOray.x + AOray.y * AOray.y);
            float2 pathTracedOffset = 1 + (normalize(randomRayDir.xz) * d0) / 40;
            float sampleHeight = heightTexture.Sample(sampleState, offsetSample + pathTracedOffset, mipMapLevel).a;
            if (sampleHeight < height)
            {
                resultAO += d0 * AOscalar;
                break;
            }
        }
    }
    
    resultAO = 1.0f - (resultAO / (rayMarchMaxLengt * N));
    
    //Ray Marching Shadow
    float resultShadow = 0.0f;
    float d0 = 0; //How far marched from origin;
    [fastopt]
    for (int j = 0; j < RMA; j++)
    {
        d0 += RML;
        float3 shadowRay = rayOrigin + normalize(directionalLightDir) * d0;
            
        float2 pathTracedOffset = (normalize(directionalLightDir.xz) * d0) / 512;
        float sampleHeight = heightTexture.Sample(sampleState, offsetSample + pathTracedOffset, mipMapLevel).a;
        if (sampleHeight < height)
        {
            resultShadow = 1.0f;
            break;
        }
    }
    
    result.color.r = resultAO;
    result.color.g = resultShadow;
    
    return result;
}