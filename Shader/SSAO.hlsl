#include "CommonFunction.hlsl"

static const int SSAOSampleCount = 14;
static const float SSAOLevel = 2.0f;

static const float4 globalSampleVec[SSAOSampleCount] =
{
    float4(+1.0f, +1.0f, +1.0f, 0.0f),
	float4(-1.0f, -1.0f, -1.0f, 0.0f),

	float4(-1.0f, +1.0f, +1.0f, 0.0f),
	float4(+1.0f, -1.0f, -1.0f, 0.0f),

	float4(+1.0f, +1.0f, -1.0f, 0.0f),
	float4(-1.0f, -1.0f, +1.0f, 0.0f),

	float4(-1.0f, +1.0f, -1.0f, 0.0f),
	float4(+1.0f, -1.0f, +1.0f, 0.0f),

	float4(-1.0f, 0.0f, 0.0f, 0.0f),
	float4(+1.0f, 0.0f, 0.0f, 0.0f),

	float4(0.0f, -1.0f, 0.0f, 0.0f),
	float4(0.0f, +1.0f, 0.0f, 0.0f),
	
	float4(0.0f, 0.0f, -1.0f, 0.0f),
	float4(0.0f, 0.0f, +1.0f, 0.0f)
};

static const float2 SSAOVertexArr[6] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(0.0f, 1.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f)
};

Texture2D globalSSAONormalMap : register(t0, space0);
Texture2D globalSSAODepthMap : register(t1, space0);
Texture2D globalSSAORandomVecMap : register(t2, space0);

SamplerState globalSampler : register(s0);
SamplerState globalsamPointClamp : register(s1);
SamplerState globalsamDepthMap : register(s2);

cbuffer MaterialData : register(b1)
{
    float OcclusionFadeStart = 2.0f;
    float OcclusionFadeEnd = 10.0f;
    float SurfaceEpsilon = 2.0f;
    float SSAORadius = 15.0f;
};

float CheckOcclusion(float distZ)
{
    float occlision = 0.0f;
    if (distZ > SurfaceEpsilon)
    {
        float fadeLength = OcclusionFadeEnd - OcclusionFadeStart;
        occlision = saturate(((OcclusionFadeEnd - distZ) / fadeLength)) * SSAOLevel;

    }
    return occlision;
}

struct VertexOutput
{
    float4 posInProjection : SV_Position;
    float3 posInView : POSITION;
    float2 posInTexture : TEXCOORD0;
};

VertexOutput VSMain(uint id : SV_VertexID)
{
    VertexOutput output;

    output.posInTexture = SSAOVertexArr[id];

    float4 vertex = float4(SSAOVertexArr[id].x, SSAOVertexArr[id].y, 0.0f, 1.0f);

    output.posInProjection = InverseTextureTransform(vertex);

    //TODO:这里留下一个疑问，NDC空间不应该进行反透视除法也就是先乘以w，然后再执行逆投影变换吗？
    float4 tempPos = mul(output.posInProjection, globalInverseProj);

    output.posInView = tempPos.xyz / tempPos.w;

    return output;
}

//TODO:这里返回4维向量，改成返回一维向量试试
float4 PSMain(VertexOutput pin) : SV_Target
{
    float3 normalInViewSpace = normalize(globalSSAONormalMap.Sample(globalsamPointClamp, pin.posInTexture).xyz);
    float depthInNDCSpace = globalSSAODepthMap.Sample(globalsamDepthMap, pin.posInTexture).r;
    
    float depthInViewSpace = NdcDepthToViewDepth(depthInNDCSpace);

    float3 currentPoint = (depthInViewSpace / pin.posInView.z) * pin.posInView;
    
    float3 randVec = 2.0f * globalSSAORandomVecMap.Sample(globalSampler, pin.posInTexture).rgb - 1.0f;
    
    float ssaoValue = 0.0f;
    
    for (int i = 0; i < SSAOSampleCount; ++i)
    {
        float3 offset = reflect(globalSampleVec[i].xyz, randVec);
        //float3 offset = normalize(globalSampleVec[i].xyz) * globalSampleVec[i].w;
        
        float flipValue = sign(dot(normalInViewSpace, offset));
        
        float3 samplePoint = currentPoint + flipValue * offset * SSAORadius;
        
        float4 samplePointInProjSpace = mul(float4(samplePoint, 1.0f), globalProjTex);
        samplePointInProjSpace /= samplePointInProjSpace.w;
        
        float depthInSamplePoint = globalSSAODepthMap.Sample(globalsamDepthMap, samplePointInProjSpace.xy).r;
        depthInSamplePoint = NdcDepthToViewDepth(depthInSamplePoint);
        
        float3 frontPoint = (depthInSamplePoint / samplePoint.z) * samplePoint;
        
        float disToPlane = max(dot(normalInViewSpace, normalize(frontPoint - currentPoint)), 0.0f);
        
        float disZ = samplePoint.z - frontPoint.z;
        
        float tempSsaoValue = disToPlane * CheckOcclusion(disZ);
        
        ssaoValue += tempSsaoValue;
    }
    
    //TODO:有些像素全黑，可以在这里加个if判断，把全黑的给一点亮度
    ssaoValue /= SSAOSampleCount;
    
    float access = 1.0f - ssaoValue;
    
    //TODO:把这里的参数和SSAOLevel作为一个参数，暴露给外部可调
    return pow(access, 8.0f);
}
