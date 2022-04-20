#include "CommonFunction.hlsl"

static const int blurRadius = 5;

static const float2 SSAOVertexArr[6] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(0.0f, 1.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f)
};

Texture2D globalSSAOMap : register(t0, space0);

Texture2D globalSSAONormalMap : register(t1, space0);

SamplerState globalSampler : register(s0);

cbuffer MaterialData : register(b1)
{
    float4 globalBlurWeights[3];
    float4 globalInvRenderTargetSize;
    bool globalHorizontalBlur;          //这里的bool值必须放在结构体的最后
};

struct VertexOuput
{
    float4 posInProjSpace : SV_Position;
    float2 posInTexSpace : TEXCOORD;
};

VertexOuput VSMain(uint id : SV_VertexID)
{
    VertexOuput output;
    
    output.posInTexSpace = SSAOVertexArr[id];

    output.posInProjSpace = InverseTextureTransform(float4(output.posInTexSpace.x, output.posInTexSpace.y, 0.0f, 1.0f));
    
    return output;
}

float4 PSMain(VertexOuput pin) : SV_Target
{
    float blurWeight[12] =
    {
        globalBlurWeights[0].x, globalBlurWeights[0].y, globalBlurWeights[0].z, globalBlurWeights[0].w,
        globalBlurWeights[1].x, globalBlurWeights[1].y, globalBlurWeights[1].z, globalBlurWeights[1].w,
        globalBlurWeights[2].x, globalBlurWeights[2].y, globalBlurWeights[2].z, globalBlurWeights[2].w
    };
    
    float2 texOffset;
    
    if(globalHorizontalBlur == true)
    {
        texOffset = float2(globalInvRenderTargetSize.x, 0.0f);
    }
    else
    {
        texOffset = float2(0.0f, globalInvRenderTargetSize.y);
    }
    
    float4 color = blurWeight[blurRadius] * globalSSAOMap.Sample(globalSampler, pin.posInTexSpace);
    
    float totalWeight = blurWeight[blurRadius];
    
    float4 sampleResult = globalSSAONormalMap.Sample(globalSampler, pin.posInTexSpace);
    float3 centerNormal = sampleResult.xyz;
    float centerDepth = NdcDepthToViewDepth(sampleResult.w);
    
    for (float i = -blurRadius; i <= blurRadius; ++i)
    {
        if (i == 0) continue;

        float2 tex = pin.posInTexSpace + i * texOffset;
        
        float4 neighborSampleResult = globalSSAONormalMap.Sample(globalSampler, tex);
        float3 neighborNormal = neighborSampleResult.xyz;
        float neighborDepth = NdcDepthToViewDepth(neighborSampleResult.w);
        
        if(dot(neighborNormal, centerNormal) >= 0.8f && abs(neighborDepth - centerDepth) <= 2.0f)
        {
            float weight = blurWeight[i + blurRadius];
            
            color += weight * globalSSAOMap.Sample(globalSampler, tex);
            
            totalWeight += weight;
        }
    }

    return color / totalWeight;
}
