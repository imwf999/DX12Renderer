#include "CommonFunction.hlsl"

#define SSAO 1

struct Light
{
    float3 lightColor;
    float fallOffStart;
    float4 direction;
    float3 position;
    float fallOffEnd;
};

Texture2D globalDiffuseTexture : register(t0, space0);
Texture2D globalNormalTexture : register(t1, space0);
Texture2D globalShadowMap : register(t2, space0);

#if SSAO
Texture2D globalSSAOMap : register(t3, space0);
#endif

SamplerState globalSampler : register(s0);
SamplerComparisonState globalComSampler : register(s1);

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
};

float3 ComputeDirectLight(float3 lightColor, float3 lightDir, float3 normal)
{
    return lightColor * max(0, dot(lightDir, normal)); //入射光量乘以朗伯余弦定理得到的直射光量
}

float3 ComputeDiffuse(float3 lightColor, float3 lightDir, float3 normal, float4 diffuseAbedo)
{
    return ComputeDirectLight(lightColor, lightDir, normal) * diffuseAbedo.rgb; //直射光量乘以漫反射反照率得到漫反射光量
}

float3 ComputeSpecular(float3 lightColor, float3 lightDir, float3 posW, float3 normal, float m)
{
    float3 viewDir = normalize(posW - globalEyePostion.xyz); 

    float3 fresnelRF = globalFresnelR0 + (1 - globalFresnelR0) * pow(1 - saturate(dot(lightDir, normal)), 5); //菲涅尔系数

    float3 halfDir = normalize(lightDir + viewDir); //直射入眼睛的那部分光线和人眼之间的微平面法线
    float roughnessFactor = (m + 8) * pow(max(dot(normal, halfDir), 0), m) / 8; //粗糙度系数

    float3 specAbedo = fresnelRF * roughnessFactor;
    specAbedo = specAbedo / (specAbedo + 1.0f); //有些高光因子会大于1，如果没有这个处理的话有些像素会呈现纯白色
    return ComputeDirectLight(lightColor, lightDir, normal) * specAbedo; //直射光量乘以菲涅尔系数乘以粗糙度得到高光光量
}

//世界空间下计算法线向量,传入的法线是世界空间下的法线
float3 ComputePointLight(Light pointLight, float3 posW, float3 normal, float m, float4 diffuseAbedo)
{
    float3 lightDir = pointLight.position - posW;
    float lightDistance = length(lightDir);
    if (lightDistance > pointLight.fallOffEnd)
        return 0.0f;
    float fallFactor = saturate((pointLight.fallOffEnd - lightDistance) / (pointLight.fallOffEnd - pointLight.fallOffStart));

    lightDir /= lightDistance;
    float3 diffuse = ComputeDiffuse(pointLight.lightColor, lightDir, normal, diffuseAbedo);
    float3 specular = ComputeSpecular(pointLight.lightColor, lightDir, posW, normal, m);

    return (diffuse + specular) * fallFactor;
}

float3 ComputeDirectionalLight(Light directionLight, float3 posW, float3 normal, float m, float4 diffuseAbedo)
{
    float3 lightDir = -directionLight.direction.xyz;
    float3 diffuse = ComputeDiffuse(directionLight.lightColor, lightDir, normal, diffuseAbedo);
    float3 specular = ComputeSpecular(directionLight.lightColor, lightDir, posW, normal, m);
    return diffuse + specular;
}

float CalculateShadowFactor(float4 shadowPosH)
{
    float depth = shadowPosH.z;

    uint width, height, numLevels;
    globalShadowMap.GetDimensions(0, width, height, numLevels);

    float dx = 1.0f / (float) width;
    float dy = 1.0f / (float) height;

    // float2 offset[9] = 
    // {
    //     float2(-dx, -dy), float2(0, -dy), float2(dx, -dy),
    //     float2(-dx, 0), float2(0, 0), float2(dx, 0),
    //     float2(-dx, dy), float2(0, dy), float2(dx, dy)
    // };

    float2 offset[25] =
    {
        float2(-dx * 2, -dy * 2), float2(-dx, -dy * 2), float2(0, -dy * 2), float2(dx, -dy * 2), float2(dx * 2, -dy * 2),
        float2(-dx * 2, -dy), float2(-dx, -dy), float2(0, -dy), float2(dx, -dy), float2(dx * 2, -dy),
        float2(-dx * 2, 0), float2(-dx, 0), float2(0, 0), float2(dx, 0), float2(dx * 2, 0),
        float2(-dx * 2, dy), float2(-dx, dy), float2(0, dy), float2(dx, dy), float2(dx * 2, dy),
        float2(-dx * 2, dy * 2), float2(-dx, dy * 2), float2(0, dy * 2), float2(dx, dy * 2), float2(dx * 2, dy * 2),
    };


    //TODO:要改
    // float2 offset[49] = 
    // {
    //     float2(-dx*3, -dy*3), float2(-dx*2, -dy*3), float2(-dx, -dy*3), float2(0, -dy*3), float2(dx, -dy*3), float2(dx*2, -dy*3), float2(dx*3, -dy*3),
    //     float2(-dx*3, -dy*2), float2(-dx*2, -dy*2), float2(-dx, -dy*2), float2(0, -dy*2), float2(dx, -dy*2), float2(dx*2, -dy*2), float2(dx*3, -dy*2),
    //     float2(-dx*3, -dy), float2(-dx*2, -dy), float2(-dx, -dy), float2(0, -dy), float2(dx, -dy), float2(dx*2, -dy), float2(dx*3, -dy),
    //     float2(-dx*3, 0), float2(-dx*2, 0), float2(-dx, 0), float2(0, 0), float2(dx, 0), float2(dx*2, 0), float2(dx*3, 0),
    //     float2(-dx*3, dy), float2(-dx*2, dy), float2(-dx, dy), float2(0, dy), float2(dx, dy), float2(dx*2, dy), float2(dx*3, dy),
    //     float2(-dx*3, dy*2), float2(-dx*2, dy*2), float2(-dx, dy*2), float2(0, dy*2), float2(dx, dy*2), float2(dx*2, dy*2), float2(dx*3, dy*2),
    //     float2(-dx*3, dy*3), float2(-dx*2, dy*3), float2(-dx, dy*3), float2(0, dy*3), float2(dx, dy*3), float2(dx*2, dy*3), float2(dx*3, dy*3),
    // };

    float persentage = 0;

    [unroll]
    for (int i = 0; i < 25; ++i)
    {
        persentage += globalShadowMap.SampleCmpLevelZero(globalComSampler, shadowPosH.xy + offset[i], depth + 0.005).r;
    }

    return persentage / 25.0f;
}

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
    float4 worldPos : POSITION0;
    float4 shadowPosH : POSITION1;
#if SSAO
    float4 ssaoPosH : POSITIONT2;
#endif
    float2 uv : TEXCOORD;
    float3 worldNormal : NORMAL0;
    float3 worldTangent : TANGENT;
};

VertexOutput VSMain(VertexInput vin)
{
    VertexOutput vout;

    vout.worldPos = mul(float4(vin.pos, 1.0f), globalWorldMatrix);
    vout.worldNormal = mul(vin.normal, (float3x3)globalNormalWorldMatrix);
    vout.worldTangent = mul( vin.tangent, (float3x3)globalWorldMatrix);

    //Note: 
    //1. 如果用的是透视投影，这里的pos在顶点着色器中经过MVP变换后是位于投影空间(或者叫齐次裁剪空间)，xy值不一定
    //是在-1到1之间，z值也是处于near和far之间而不是在0到1之间，需要经过透视除法之后才位于NDC空间，此时
    //才有-1 < x,y < 1, 0 < z < 1，但如果用的是正交投影，在经过MVP变换后就已经位于NDC空间中了
    //2. 如果near和far间隔太远的话会导致透视除法之后z值集中在0到1的一小部分区域内，例如在这个sponza场景
    //中一开始把远近平面设为1和8000，调试时发现深度值都是0.999xxx，可能会存在精度不够的问题，具体描述位于龙书163页
    //3. pos在作为顶点着色器的输出时是位于投影空间的(也必须位于投影空间，因为有SV_POSITION修饰)，当作为像素
    //着色器的输入时是已经经过透视除法和视口变换后的值，xy对应屏幕上的像素点坐标(但不完全是整数，小数部分还多了0.5)
    //z值对应像素深度，位于0到1之间，w值是依然投影空间中的w值，也就是硬件的透视除法和视口变换都没有影响到w
    vout.pos = mul(vout.worldPos, globalViewProj);

    vout.uv = vin.uv;
    vout.shadowPosH = mul(float4(vin.pos, 1.0f), globalShadowMVP);

#if SSAO
    vout.ssaoPosH = mul(vout.worldPos, globalViewProj);
#endif

    return vout;
}

float4 PSMain(VertexOutput pin) : SV_TARGET
{

    pin.worldNormal = normalize(pin.worldNormal);

    pin.worldTangent = normalize(pin.worldTangent);
    float3 worldBinormal = cross(pin.worldNormal, pin.worldTangent);

    float3x3 worldTBN = float3x3(pin.worldTangent, worldBinormal, pin.worldNormal);

    float4 tempNormal = globalNormalTexture.Sample(globalSampler, pin.uv);
    tempNormal = 2.0f * tempNormal - 1.0f;

    float3 worldNormal = mul(tempNormal.rgb, worldTBN);

    float shadowFactor = 1.0f;
    shadowFactor = CalculateShadowFactor(pin.shadowPosH);

    float4 diffuseAbedo = globalDiffuseTexture.Sample(globalSampler, pin.uv) * globalDiffuse;
    
#if SSAO
    pin.ssaoPosH /= pin.ssaoPosH.w;
    float4 texCoord = TextureTranform(pin.ssaoPosH);
    float ambientAccess = globalSSAOMap.Sample(globalSampler, texCoord.xy).r;
    float3 ambient = globalAmbient.rgb * diffuseAbedo.rgb * ambientAccess;
#else
    float3 ambient = globalAmbient.rgb * diffuseAbedo.rgb;
#endif

    float m = globalShininess * 256 * tempNormal.a; 

    float3 directionalLight = { 0.0f, 0.0f, 0.0f };

    directionalLight = ComputeDirectionalLight(globalDirectionalLight, pin.worldPos.xyz, worldNormal, m, diffuseAbedo);

    directionalLight *= shadowFactor;
    
    float3 totalColor = ambient + directionalLight;
    float4 finalColor = float4(totalColor, diffuseAbedo.w);

    return finalColor;
}