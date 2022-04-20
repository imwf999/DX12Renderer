#include "CommonData.hlsl"

cbuffer MaterialData : register(b1)
{
    float4x4 globalWorldMatrix;
};

struct VertexInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexOutput
{
    float4 posH : SV_POSITION;
    float3 normal_W : NORMAL;
};

VertexOutput VSMain(VertexInput input)
{
    VertexOutput output;
    
    float4 pos_W = mul(float4(input.pos, 1.0f), globalWorldMatrix);

    output.posH = mul(pos_W, globalViewProj);

    output.normal_W = mul(input.normal, (float3x3)globalWorldMatrix);

    return output;
}

float4 PSMain(VertexOutput input) :SV_TARGET
{
    input.normal_W = normalize(input.normal_W);

    float4 resNormal;
    
    resNormal.xyz = mul(input.normal_W, (float3x3)globalView);

    return resNormal;
}