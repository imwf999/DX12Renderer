Texture2D globalRenderTexture : register(t0);

SamplerState globalSampler : register(s0);

cbuffer ConstBuffer : register(b0)
{
    float4x4 mvpMatrix;
}

struct VertexInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VertexOutput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VertexOutput RTRVSMain(VertexInput vin)
{
    VertexOutput vout;

    vout.pos = mul( float4(vin.pos, 1), mvpMatrix);
    vout.uv = vin.uv;

    return vout;
}

float4 RTRPSMain(VertexOutput pin) :SV_Target
{
    return globalRenderTexture.Sample(globalSampler, pin.uv);
}