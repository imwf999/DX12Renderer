#include "CommonData.hlsl"

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
};

VertexOutput VSMain(VertexInput vin)
{
    VertexOutput vout;

    float4 posW = mul(float4(vin.pos, 1), globalWorldMatrix);

    vout.pos = mul(posW, globalViewProjForShadow);

    return vout;
}

void PSMain(VertexOutput pin)
{

}
