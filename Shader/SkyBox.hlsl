#include "CommonData.hlsl"

TextureCube globalCubeMap : register(t0, space0);

SamplerState globalSampler : register(s0);

cbuffer MaterialData : register(b1)
{
    float4x4 globalWorldMatrix;
};

struct VertexInput
{
    float3 pos : POSITION;
};

struct VertexOutput
{
    float4 pos : SV_Position;
    float3 localPos : POSITION;
};

VertexOutput VSMain(VertexInput vin)
{
    VertexOutput vout;

    vout.localPos = vin.pos;

    float4 worldPos = mul(float4(vin.pos, 1), globalWorldMatrix);

    worldPos.xyz += globalEyePostion;

    vout.pos = mul(worldPos, globalViewProj).xyww;

    return vout;
}

float4 PSMain(VertexOutput pin) : SV_Target
{
    return globalCubeMap.Sample(globalSampler, pin.localPos);
}