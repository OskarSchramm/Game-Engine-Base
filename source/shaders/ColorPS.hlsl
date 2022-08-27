#include "ShaderStructs.hlsli"
#include "ShaderCommon.hlsli"
#include "CommonFunctions.hlsli"
#include "PBRFunctions.hlsli"

#define USE_LM true

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    
    float LMAO = 0.0f;
    float3 dirLightLM = directionalLightColor.xyz;
    float2 lightMap;
    float2 sampleOffset = float2(1 + input.lmCoord.x / 2, 1 + input.lmCoord.y / 2);
    
    //lightmapStuff
    if (USE_LM)
    {
        dirLightLM *= lmTexture.Sample(sampleState, sampleOffset).g;

        //blur ao
        float4 offsets = float4(-1.5, -0.5, 0.5, 1.5);
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                float2 tc;
                tc.x = (1 - (input.lmCoord.x * -1.0f)) / 2 + offsets[j];
                tc.y = (1 - (input.lmCoord.y * -1.0f)) / 2 + offsets[i];
                LMAO += lmTexture.Sample(sampleState, tc).r;
            }
        }
    
        LMAO /= 16.0;
    }
    
    
    float3 toEye = normalize(cameraPosition.xyz - input.worldPosition.xyz);
    
    //TBN Matrix in Object Space
    float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
    
    //Albedos
    float4 grassA = grassTexture.Sample(sampleState, input.uv.xy).rgba;
    float4 rockA  = rockTexture.Sample(sampleState, input.uv.xy).rgba;
    float4 snowA  = snowTexture.Sample(sampleState, input.uv.xy).rgba;
    
    //Normals
    float4 grassNIn = grassNormal.Sample(sampleState, input.uv).rgba;
    float4 rockNIn = rockNormal.Sample(sampleState, input.uv).rgba;
    float4 snowNIn = snowNormal.Sample(sampleState, input.uv).rgba;
    float3 grassN = expandNormal(grassNIn);
    float3 rockN = expandNormal(rockNIn);
    float3 snowN = expandNormal(snowNIn);
    
    //Materials
    float3 grassM = grassMaterial.Sample(sampleState, input.uv.xy).rgba; //r = metalness, g = roughness, b = emissive
    float3 rockM  = rockMaterial.Sample(sampleState, input.uv.xy).rgba; //r = metalness, g = roughness, b = emissive
    float3 snowM  = snowMaterial.Sample(sampleState, input.uv.xy).rgba; //r = metalness, g = roughness, b = emissive
    grassM.g = smoothstep(0.85, 1.0, grassM.g);
    rockM.g = smoothstep(0.85, 1.0, rockM.g);
    snowM.g = smoothstep(0.85, 1.0, snowM.g);
    
    //Lerping terrain
    float slopeBlend  = smoothstep(0.7f, 1.0f, input.normal.y);
    float heightBlend = smoothstep(-0.05f, 0.25f, input.worldPosition.y);
    
    float ao = lerp(rockNIn.b, lerp(grassNIn.b, snowNIn.b, heightBlend), slopeBlend);
    
    float3 albedo = lerp(rockA, lerp(grassA, snowA, heightBlend), slopeBlend).rgb;
    float3 normal = lerp(rockN, lerp(grassN, snowN, heightBlend), slopeBlend);  
    float3 material = lerp(rockM, lerp(grassM, snowM, heightBlend), slopeBlend);
    normal = normalize(mul(normal, TBN));
    
    //LightColor
    float ambientScalar = 0.05f;
    float4 sky    = saturate(0.5f - 0.55f * input.normal.y) * skyColor    * ambientScalar;
    float4 ground = saturate(0.5f + 0.35f * input.normal.y) * groundColor * ambientScalar;
    
    //Calculate light
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, material.r);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - material.r);
    
    float3 ambiance = EvaluateAmbiance(
		LMAO, cubeMap, normal, input.normal,
		toEye, material.g,
		ao, diffuseColor, specularColor
	);

    float3 dirL = EvaluateDirectionalLight(
		diffuseColor, specularColor, normal, material.g,
		dirLightLM, directionalLightDir, toEye.xyz
	);
    
    //float3 emissiveAlbedo = albedo.rgb * material.b;
    float3 radiance = float3(sky.xyz + ground.xyz) + dirL + ambiance;
    
    float3 finalColor = radiance;
    
    if (USE_LM)
    {
        result.color.rgb = lmTexture.Sample(sampleState, sampleOffset).r;
        result.color.rgb = tonemap_s_gamut3_cine(finalColor);
    }
    else
        result.color.rgb = tonemap_s_gamut3_cine(finalColor);
    
    result.color.a = 1.0f;
    
    return result;
}