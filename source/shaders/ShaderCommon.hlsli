Texture2D grassTexture : register(t0);
Texture2D rockTexture : register(t1);
Texture2D snowTexture : register(t2);

Texture2D grassNormal : register(t3);
Texture2D rockNormal : register(t4);
Texture2D snowNormal : register(t5);

Texture2D grassMaterial : register(t6);
Texture2D rockMaterial : register(t7);
Texture2D snowMaterial : register(t8);

TextureCube cubeMap : register(t9);

Texture2D lmTexture : register(t13);

SamplerState sampleState : register(s0);

cbuffer FrameBuffer : register(b0)
{
    float4x4 modelToClipMatrix;
    float4x4 cameraMatrix;
    float3 cameraPosition;
    float totalTime;
};

cbuffer ObjectBuffer : register(b1)
{
    float4x4 modelMatrix;
};

cbuffer LightBuffer : register(b2)
{
    float4 directionalLightColor;
    float4 skyColor;
    float4 groundColor;
    float3 directionalLightDir;
    float padding0;
}

cbuffer SettingsBuffer : register(b3)
{
    float aResolutionWidth;
    float aResolutionHeight;
    float waterHeight;
    float padding;
}