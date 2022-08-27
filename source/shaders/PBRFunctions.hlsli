//// BRDF Globals
#define FLT_EPSILON 1.192092896e-07f
#define nMipOffset 3
#define PI 3.14159265358979323846f

float bias(float value, float b)
{
    return (b > 0.0) ? pow(abs(value), log(b) / log(0.5)) : 0.0f;
}

float gain(float value, float g)
{
    return 0.5 * ((value < 0.5) ? bias(2.0 * value, 1.0 - g) : (2.0 - bias(2.0 - 2.0 * value, 1.0 - g)));
}

float RoughnessFromPerceptualRoughness(float perceptualRoughness)
{
    return perceptualRoughness * perceptualRoughness;
}

float PerceptualRougnessFromRoughness(float roughness)
{
    return sqrt(max(0.0, roughness));
}

float SpecularPowerFromPerceptualRoughness(float perceptualRoughness)
{
    float roughness = RoughnessFromPerceptualRoughness(perceptualRoughness);
    return (2.0 / max(FLT_EPSILON, roughness * roughness)) - 2.0;
}

float PerceptualRougnessFromSpecularPower(float specularPower)
{
    float roughness = sqrt(2.0 / (specularPower + 2.0));
    return PerceptualRougnessFromRoughness(roughness);
}

float BurleyToMip(float fPerceptualRoughness, int nMips, float NdotR)
{
    float specPower = SpecularPowerFromPerceptualRoughness(fPerceptualRoughness);
    specPower /= (4 * max(NdotR, FLT_EPSILON));
    float scale = PerceptualRougnessFromSpecularPower(specPower);
    return scale * (nMips - 1 - nMipOffset);
}

float3 GetSpecularDominantDir(float3 vN, float3 vR, float roughness)
{
    float invRough = saturate(1 - roughness);
    float alpha = invRough * (sqrt(invRough) + roughness);

    return lerp(vN, vR, alpha);
}

float GetReductionInMicrofacets(float perceptualRoughness)
{
    float roughness = RoughnessFromPerceptualRoughness(perceptualRoughness);

    return 1.0 / (roughness * roughness + 1.0);
}

float EmpiricalSpecularAO(float ao, float perceptualRoughness)
{
    float smooth = 1 - perceptualRoughness;
    float specAO = gain(ao, 0.5 + max(0.0, smooth * 0.4));

    return min(1.0, specAO + lerp(0.0, 0.5, smooth * smooth * smooth * smooth));
}

float ApproximateSpecularSelfOcclusion(float3 vR, float3 vertNormalNormalized)
{
    const float fadeParam = 1.3;
    float rimmask = clamp(1 + fadeParam * dot(vR, vertNormalNormalized), 0.0, 1.0);
    rimmask *= rimmask;

    return rimmask;
}

float3 Diffuse(float3 pAlbedo)
{
    return pAlbedo / PI;
}

float NormalDistribution_GGX(float a, float NdH)
{
    // Isotropic ggx
    float a2 = a * a;
    float NdH2 = NdH * NdH;

    float denominator = NdH2 * (a2 - 1.0f) + 1.0f;
    denominator *= denominator;
    denominator *= PI;

    return a2 / denominator;
}

float Geometric_Smith_Schlick_GGX(float a, float NdV, float NdL)
{
    // Smith Schlick-GGX
    float k = a * 0.5f;
    float GV = NdV / (NdV * (1 - k) + k);
    float GL = NdL / (NdL * (1 - k) + k);

    return GV * GL;
}

float3 Fresnel_Schlick(float3 specularColor, float3 h, float3 v)
{
    return (specularColor + (1.0f - specularColor) * pow((1.0f - saturate(dot(v, h))), 5));
}

float3 Specular(float3 specularColor, float3 h, float3 v, float a, float NdL, float NdV, float NdH)
{
    return ((NormalDistribution_GGX(a, NdH) * Geometric_Smith_Schlick_GGX(a, NdV, NdL)) * Fresnel_Schlick(specularColor, h, v)) / (4.0f * NdL * NdV + 0.0001f);
}

float3 EvaluateAmbiance(float lmAO, TextureCube lysBurleyCube, float3 vN, float3 VNUnit, float3 toEye, float perceptualRoughness, float ao, float3 dfcol, float3 spccol)
{
    int numMips = GetNumMips(lysBurleyCube);
    const int nrBrdMips = numMips - nMipOffset;
    float VdotN = saturate(dot(toEye, vN)); //clamp(dot(toEye, vN), 0.0, 1.0f);
    const float3 vRorg = 2 * vN * VdotN - toEye;

    float3 vR = GetSpecularDominantDir(vN, vRorg, RoughnessFromPerceptualRoughness(perceptualRoughness));
    float RdotNsat = saturate(dot(vN, vR));

    float mipLevel = BurleyToMip(perceptualRoughness, numMips, RdotNsat);

    float3 specRad = lysBurleyCube.SampleLevel(sampleState, vR, mipLevel).xyz;
    float3 diffRad = lysBurleyCube.SampleLevel(sampleState, vN, (float) (nrBrdMips - 1)).xyz;
    
    specRad *= lmAO; //ADDED Lightmap
    diffRad *= lmAO; //ADDED Lightmap

    float fT = 1.0 - RdotNsat;
    float fT5 = fT * fT;
    fT5 = fT5 * fT5 * fT;
    spccol = lerp(spccol, (float3) 1.0, fT5);

    float fFade = GetReductionInMicrofacets(perceptualRoughness);
    fFade *= EmpiricalSpecularAO(ao, perceptualRoughness);
    fFade *= ApproximateSpecularSelfOcclusion(vR, VNUnit);

    float3 ambientdiffuse = ao * dfcol * diffRad;
    float3 ambientspecular = fFade * spccol * specRad;
    return ambientdiffuse + ambientspecular;
}

float3 EvaluateDirectionalLight(float3 albedoColor, float3 specularColor, float3 normal, float roughness, float3 lightColor, float3 lightDir, float3 viewDir)
{
    // Compute som useful values
    float NdL = saturate(dot(normal, lightDir));
    float lambert = NdL; // Angle attenuation
    float NdV = saturate(dot(normal, viewDir));
    float3 h = normalize(lightDir + viewDir);
    float NdH = saturate(dot(normal, h));
    float VdH = saturate(dot(viewDir, h));
    float LdV = saturate(dot(lightDir, viewDir));
    float a = max(0.001f, roughness * roughness);

    float3 cDiff = Diffuse(albedoColor);
    float3 cSpec = Specular(specularColor, h, viewDir, a, NdL, NdV, NdH);
    return saturate(lightColor * lambert * (cDiff * (1.0 - cSpec) + cSpec) * PI);
}

float3 EvaluatePointLight(float3 albedoColor, float3 specularColor, float3 normal, float roughness,
    float3 lightColor, float lightIntensity, float lightRange, float3 lightPos, float3 viewDir, float3 pixelPos)
{
    // Compute som useful values
    float3 lightDir = lightPos.xyz - pixelPos.xyz;
    float lightDistance = length(lightDir);
    lightDir = normalize(lightDir);
	
    float NdL = saturate(dot(normal, lightDir));
    float lambert = NdL; // Angle attenuation
    float NdV = saturate(dot(normal, viewDir));
    float3 h = normalize(lightDir + viewDir);
    float NdH = saturate(dot(normal, h));
    float a = max(0.001f, roughness * roughness);

    float3 cDiff = Diffuse(albedoColor);
    float3 cSpec = Specular(specularColor, h, viewDir, a, NdL, NdV, NdH);

    float linearAttenuation = lightDistance / lightRange;
    linearAttenuation = 1.0f - linearAttenuation;
    linearAttenuation = saturate(linearAttenuation);
    float physicalAttenuation = saturate(1.0f / (lightDistance * lightDistance));
    float ue4Attenuation = ((pow(saturate(1 - pow(lightDistance / lightRange, 4.0f)), 2.0f)) / (pow(lightDistance, 2.0f) + 1)); // Unreal Engine 4 attenuation
    float attenuation = lambert * linearAttenuation * physicalAttenuation;
    attenuation = ue4Attenuation * lambert;

    return saturate(lightColor * lightIntensity * attenuation * ((cDiff * (1.0 - cSpec) + cSpec) * PI));
}

float3 EvaluateSpotLight(float3 albedoColor, float3 specularColor, float3 normal,
    float roughness, float3 lightColor, float lightIntensity, float lightRange,
    float3 lightPos, float3 lightDir, float outerAngle, float innerAngle, float3 viewDir, float3 pixelPos)
{
    float3 toLight = lightPos.xyz - pixelPos.xyz;
    float lightDistance = length(toLight);
    toLight = normalize(toLight);

    float NdL = saturate(dot(normal, toLight));
    float lambert = NdL; // Angle attenuation
    float NdV = saturate(dot(normal, viewDir));
    float3 h = normalize(toLight + viewDir);
    float NdH = saturate(dot(normal, h));
    float a = max(0.001f, roughness * roughness);

    float3 cDiff = Diffuse(albedoColor);
    float3 cSpec = Specular(specularColor, h, viewDir, a, NdL, NdV, NdH);

    float cosOuterAngle = cos(outerAngle);
    float cosInnerAngle = cos(innerAngle);
    float3 lightDirection = lightDir;

    // Determine if pixel is within cone.
    float theta = dot(toLight, normalize(-lightDirection));
	// And if we're in the inner or outer radius.
    float epsilon = cosInnerAngle - cosOuterAngle;
    float intensity = clamp((theta - cosOuterAngle) / epsilon, 0.0f, 1.0f);
    intensity *= intensity;
	
    float ue4Attenuation = ((pow(saturate(1 - pow(lightDistance / lightRange, 4.0f)), 2.0f)) / (pow(lightDistance, 2.0f) + 1)); // Unreal Engine 4 attenuation
    float finalAttenuation = lambert * intensity * ue4Attenuation;

    return saturate(lightColor * lightIntensity * lambert * finalAttenuation * ((cDiff * (1.0 - cSpec) + cSpec) * PI));
}