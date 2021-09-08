#include "CommonData.hlsl"

struct VertexInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VertexOutput
{
    float4 pos : SV_Position;
};

VertexOutput VSMain(VertexInput vin)
{
    VertexOutput vout;

    float4 posW = mul(float4(vin.pos, 1), globalWorldMatrix);

    vout.pos = mul(posW, globalViewProj);

    return vout;
}

void PSMain(VertexOutput pin)
{

}
