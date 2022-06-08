#include "ShaderStructs.hlsli"
#include "ShaderCommon.hlsli"
#include "CommonFunctions.hlsli"
#include "PBRFunctions.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    
    float3 toEye = normalize(cameraPosition.xyz - input.worldPosition.xyz);
    
    //TBN Matrix in Object Space
    float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
    
    //Albedos
    float4 grassA = grassTexture.Sample(sampleState, input.uv.xy).rgba;
    float4 rockA  = rockTexture.Sample(sampleState, input.uv.xy).rgba;
    float4 snowA  = snowTexture.Sample(sampleState, input.uv.xy).rgba;
    
    //Normals
    float3 grassN = expandNormal(grassNormal.Sample(sampleState, input.uv).rgba); //b = AO
    float3 rockN  = expandNormal(rockNormal.Sample(sampleState, input.uv).rgba);  //b = AO
    float3 snowN  = expandNormal(snowNormal.Sample(sampleState, input.uv).rgba);  //b = AO
    
    //Materials
    float3 grassM = grassMaterial.Sample(sampleState, input.uv.xy).rgba; //r = metalness, g = roughness, b = emissive
    float3 rockM  = rockMaterial.Sample(sampleState, input.uv.xy).rgba; //r = metalness, g = roughness, b = emissive
    float3 snowM  = snowMaterial.Sample(sampleState, input.uv.xy).rgba; //r = metalness, g = roughness, b = emissive
    
    //Lerping terrain
    float slopeBlend  = smoothstep(0.7f, 1.0f, input.normal.y);
    float heightBlend = smoothstep(-0.05f, 0.25f, input.worldPosition.y);
    
    float3 albedo = lerp(rockA, lerp(grassA, snowA, heightBlend), slopeBlend).rgb;
    float3 normal = lerp(rockN, lerp(grassN, snowN, heightBlend), slopeBlend);
    float3 material = lerp(rockM, lerp(grassM, snowM, heightBlend), slopeBlend);
    
    normal = normalize(mul(normal, TBN));
    
    //LightColor
    float ambientScalar = 0.2f;
    float4 sky    = saturate(0.5f - 0.55f * input.normal.y) * skyColor    * ambientScalar;
    float4 ground = saturate(0.5f + 0.35f * input.normal.y) * groundColor * ambientScalar;
    
    //Calculate light
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, material.r);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - material.r);
    
    float3 ambiance = EvaluateAmbiance(
		cubeMap, normal, input.normal,
		toEye, material.g,
		normal.b, diffuseColor, specularColor
	);

    float3 dirL = EvaluateDirectionalLight(
		diffuseColor, specularColor, normal, material.g,
		directionalLightColor.xyz, directionalLightDir, toEye.xyz
	);
    
    //float3 emissiveAlbedo = albedo.rgb * material.b;
    float3 radiance = float3(sky.xyz + ground.xyz) + ambiance + dirL;
    
    float3 finalColor = radiance;
    result.color.rgb = tonemap_s_gamut3_cine(finalColor);
    result.color.a   = 1.0f;
    
    return result;
}