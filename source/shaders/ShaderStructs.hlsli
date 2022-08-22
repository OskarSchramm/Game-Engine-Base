struct VertexInputType
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    
    //ADDED
    float2 lmCoord : LMCOORD;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float3 worldPosition : WORLDPOSITION;
    float clip : SV_ClipDistance;
    
    //ADDED
    float2 lmCoord : LMCOORD;
};

struct LightmapInputType
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float3 worldPosition : WORLDPOSITION;
    float2 lmCoord : TEXCOORD;
};


struct PixelOutput
{
    float4 color : SV_TARGET;
};