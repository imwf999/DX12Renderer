#include "CommonFunction.hlsl"

struct Light
{
    float3 lightColor;
    float fallOffStart;
    float4 direction;
    float4 position;
};

static const float4 globalSampleVec[54] =
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
	float4(0.0f, 0.0f, +1.0f, 0.0f),
    
    //右边面上的八个点
	float4(1.0f, 0.33333f, 0.33333f, 0.0f),
	float4(1.0f, 1.0f, 0.33333f, 0.0f),

	float4(1.0f, 0.33333f, -0.33333f, 0.0f),
	float4(1.0f, 1.0f, -0.333333f, 0.0f),

	float4(1.0f, -0.33333f, 0.33333f, 0.0f),
	float4(1.0f, -1.0f, 0.33333f, 0.0f),

	float4(1.0f, -0.33333f, -0.33333f, 0.0f),
	float4(1.0f, -1.0f, -0.333333f, 0.0f),

	//前面的八个点
	float4(0.33333f, 0.33333f, 1.0f, 0.0f),
	float4(0.33333f, 1.0f, 1.0f, 0.0f),

	float4(-0.33333f, 0.33333f, 1.0f, 0.0f),
	float4(-0.33333f, 1.0f, 1.0f, 0.0f),

	float4(0.33333f, -0.33333f, 1.0f, 0.0f),
	float4(0.33333f, -1.0f, 1.0f, 0.0f),

	float4(-0.33333f, -0.33333f, 1.0f, 0.0f),
	float4(-0.33333f, -1.0f, 1.0f, 0.0f),

	//左边面上的八个点
	float4(-1.0f, 0.33333f, 0.33333f, 0.0f),
	float4(1.0f, 1.0f, 0.33333f, 0.0f),

	float4(-1.0f, 0.33333f, -0.33333f, 0.0f),
	float4(-1.0f, 1.0f, -0.333333f, 0.0f),

	float4(-1.0f, -0.33333f, 0.33333f, 0.0f),
	float4(-1.0f, -1.0f, 0.33333f, 0.0f),

	float4(-1.0f, -0.33333f, -0.33333f, 0.0f),
	float4(-1.0f, -1.0f, -0.333333f, 0.0f),

	//后面的八个点
	float4(0.33333f, 0.33333f, -1.0f, 0.0f),
	float4(0.33333f, 1.0f, -1.0f, 0.0f),

	float4(-0.33333f, 0.33333f, -1.0f, 0.0f),
	float4(-0.33333f, 1.0f, -1.0f, 0.0f),

	float4(0.33333f, -0.33333f, -1.0f, 0.0f),
	float4(0.33333f, -1.0f, -1.0f, 0.0f),

	float4(-0.33333f, -0.33333f, -1.0f, 0.0f),
	float4(-0.33333f, -1.0f, -1.0f, 0.0f),

	//上面的四个点
	float4(0.3333f, +1.0f, 0.3333f, 0.0f),
	float4(-0.33333f, 1.0f, 0.33333f, 0.0f),

	float4(0.3333f, +1.0f, -0.3333f, 0.0f),
	float4(-0.33333f, 1.0f, -0.33333f, 0.0f),

	//下面四个点
	float4(0.3333f, -1.0f, 0.3333f, 0.0f),
	float4(-0.33333f, -1.0f, 0.33333f, 0.0f),

	float4(0.3333f, -1.0f, -0.3333f, 0.0f),
	float4(-0.33333f, -1.0f, -0.33333f, 0.0f)
};

//SSR采样数
static const int globalSSRSampleCount = 20;

Texture2D globalGBufferDiffuse : register(t0, space0);
Texture2D globalGBufferNormal : register(t1, space0);       //世界空间下的法线
Texture2D globalGBufferDepth : register(t2, space0);
Texture2D globalGBufferDirectLight : register(t3, space0);
Texture2D globalShadowMap : register(t4, space0);
Texture2D globalSSAORandomVecMap : register(t5, space0);

SamplerState globalSampler : register(s0);
SamplerComparisonState globalComSampler : register(s1);
SamplerState globalsamPointClamp : register(s2);
SamplerState globalsamDepthMap : register(s3);

cbuffer MaterialData : register(b1)
{
    float4x4 globalWorldMatrix;
    float4x4 globalNormalWorldMatrix;
    float4 globalDiffuse;
    float4 globalAmbient;
    float4 globalSpecular;
    float3 globalFresnelR0;
    float globalShininess;
    Light globalDirectionalLight;
	bool globalOpenIndirectLight;
};


float2 GetScreenCoord(float4 worldPos)
{
    float4 projPos = mul(worldPos, globalViewProj);
    projPos /= projPos.w;
    projPos = TextureTranform(projPos);
    return projPos.xy;
}

float GetDepthFromWorldPosition(float4 worldPos)
{
    float4 projPos = mul(worldPos, globalViewProj);
    projPos /= projPos.w;
    return projPos.z;
}

//TODO:加入UI后硬编码数字都需要改成变量动态修改
bool RayMarch(float3 ori, float3 dir, out float3 hitPos)
{
    float step = 50.0f;
    float3 currentPoint = ori;
    
    for (int i = 0; i < 20; ++i)
    {
        float3 testPoint = currentPoint + step * dir;
        
        float testDepth = GetDepthFromWorldPosition(float4(testPoint, 1.0f));       //光线末端点的深度
        float bufferDepth = globalGBufferDepth.Sample(globalsamDepthMap, GetScreenCoord(float4(testPoint, 1.0f)));  //当前像素最小深度
        
        //大于1或者小于0表示光线已经出了视锥体，没有交点
		if (bufferDepth > 0.999999f || bufferDepth < 0.0000001f)
			return false;
        
		float deltaDepth = testDepth - bufferDepth;
        
        //如果深度之差小于0或者大于0.1，表示没有和物体相交
        if(deltaDepth < 0.0f || deltaDepth >0.1f)
        {
            currentPoint = testPoint;
            continue;
        }
        else if(deltaDepth > 0.002)     //小于0.1大于0.002表示相交了，步长减小一半重新试探
        {
            step /= 2.0f;
        }
        else        //小于0.002表示光线末端点就是和物体的交点。
        {
            hitPos = testPoint;
            return true;
        }
    }
    return false;
}

struct VertexInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexOuput
{
    float4 pos : SV_Position;
    float4 posW : POSITIONT0;
    float2 uv : TEXCOORD;
};

VertexOuput VSMain(VertexInput vin)
{
    VertexOuput output;
    output.posW = mul(float4(vin.pos, 1.0f), globalWorldMatrix);
    output.pos = mul(output.posW, globalViewProj);
    output.uv = vin.uv;
    return output;
}

float4 PSMain(VertexOuput pin) : SV_Target
{
    float3 indirectLight = { 0.0f, 0.0f, 0.0f };
    float3 directLight = { 0.0f, 0.0f, 0.0f };
    
    float2 uv = GetScreenCoord(pin.posW);
    float3 worldNormal = globalGBufferNormal.Sample(globalsamPointClamp, uv).xyz;
    float4 diffuseAlbedo = globalGBufferDiffuse.Sample(globalsamPointClamp, uv);
    directLight = globalGBufferDirectLight.Sample(globalsamPointClamp, uv).xyz;
    
	if (globalOpenIndirectLight == false)
		return float4(directLight, diffuseAlbedo.a);
    
	float3 randVec = 2.0f * globalSSAORandomVecMap.Sample(globalSampler, GetScreenCoord(pin.posW)).xyz - 1.0f;
    
    for (int i = 0; i < globalSSRSampleCount; ++i)  
    {
        float3 offset = normalize(reflect(globalSampleVec[i].xyz, randVec));
        float flipValue = sign(dot(worldNormal, offset));
        offset *= flipValue;
        
        float3 hitPos;
        if (RayMarch(pin.posW.xyz, offset, hitPos))
        {
            float2 uv = GetScreenCoord(float4(hitPos, 1.0f));
            float3 lightColor = globalGBufferDirectLight.Sample(globalsamPointClamp, uv).xyz;
            float m = globalShininess * 256.0f;
            float3 lightDir = normalize(pin.posW.xyz - hitPos);
            float3 tempDirectLight = ComputeDirectionalLight(lightDir, lightColor, pin.posW.xyz, worldNormal, m, diffuseAlbedo, globalFresnelR0);
            float distance = length(pin.posW.xyz - hitPos);
            float weight = 1.0f - distance / 1000.0f;
            indirectLight += tempDirectLight * weight;
        }
    }
    //indirectLight = pow(indirectLight, 2.0f);
   
    indirectLight /= 2.0f;
    
    return float4(directLight + indirectLight, diffuseAlbedo.a);
}