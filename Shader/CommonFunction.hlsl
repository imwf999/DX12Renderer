#include "CommonData.hlsl"

float3 ComputeDirectLight(float3 lightColor, float3 lightDir, float3 normal)
{
    return lightColor * max(0, dot(lightDir, normal));          //入射光量乘以朗伯余弦定理后得到的直射光量
}

float3 ComputeDiffuse(float3 lightColor, float3 lightDir, float3 normal, float4 diffuseAbedo)
{
    return ComputeDirectLight(lightColor, lightDir, normal) * diffuseAbedo.rgb;         //直射光量乘以漫反射反照率得到漫反射光量
}

float3 ComputeSpecular(float3 lightColor, float3 lightDir, float3 posW, float3 normal, float m)
{
    float3 viewDir = normalize(posW - globalEyePostion);             //人眼观察方向

    float3 fresnelRF = globalFresnelR0 + (1 - globalFresnelR0) * pow(1 - saturate(dot(lightDir, normal)), 5);       //菲涅尔系数

    float3 halfDir = normalize(lightDir + viewDir);             //直射入眼睛的那部分光线和人眼之间的微平面法线
    float roughnessFactor = (m + 8) * pow(max(dot(normal, halfDir), 0), m) / 8;             //粗糙度系数

    float3 specAbedo = fresnelRF * roughnessFactor;
    specAbedo = specAbedo / (specAbedo + 1.0f);                 //有些高光因子会大于1，如果没有这个处理的话有些像素会呈现纯白色
    return ComputeDirectLight(lightColor, lightDir, normal) * specAbedo;      //直射光量乘以菲涅尔系数乘以粗糙度得到高光光量
}

//世界空间下计算法线向量,传入的法线是世界空间下的法线
float3 ComputePointLight(Light pointLight, float3 posW, float3 normal, float m, float4 diffuseAbedo)
{
    float3 lightDir = pointLight.position - posW;
    float lightDistance = length(lightDir);
    if(lightDistance > pointLight.fallOffEnd)
    return 0.0f;
    float fallFactor = saturate((pointLight.fallOffEnd - lightDistance) / (pointLight.fallOffEnd - pointLight.fallOffStart));

    lightDir /= lightDistance;
    float3 diffuse = ComputeDiffuse(pointLight.lightColor, lightDir, normal, diffuseAbedo);
    float3 specular = ComputeSpecular(pointLight.lightColor, lightDir, posW, normal, m);

    return (diffuse + specular) * fallFactor;
}

float3 ComputeDirectionalLight(Light directionLight, float3 posW, float3 normal, float m, float4 diffuseAbedo)
{
    float3 lightDir = -directionLight.direction;
    float3 diffuse = ComputeDiffuse(directionLight.lightColor, lightDir, normal, diffuseAbedo);
    float3 specular = ComputeSpecular(directionLight.lightColor, lightDir, posW, normal, m);
    return diffuse + specular;
}

float CalculateShadowFactor(float4 shadowPosH)
{
    shadowPosH.xyz = shadowPosH.xyz / shadowPosH.w;

    float depth = shadowPosH.z;

    uint width, height, numLevels;
    globalShadowMap.GetDimensions(0, width, height, numLevels);

    float dx = 1.0f / (float)width;
    float dy = 1.0f / (float)height;

    float2 offset[9] = 
    {
        float2(-dx, -dy), float2(0, -dy), float2(dx, -dy),
        float2(-dx, 0), float2(0, 0), float2(dx, 0),
        float2(-dx, dy), float2(0, dy), float2(dx, dy)
    };
    
    float persentage = 0;

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        persentage += globalShadowMap.SampleCmpLevelZero(globalComSampler, shadowPosH.xy + offset[i], depth).r;
    }

    return persentage / 9.0f;
}

#ifdef POINTSHADOW
// float ComputePointShadowFactor(float4 shadowPosH, uint shadowMapIndex)
// {
//     shadowPosH.xyz = shadowPosH.xyz / shadowPosH.w;
//     if(shadowPosH.x <= -1 || shadowPosH.x >= 1.000 || shadowPosH.y <= -1 || shadowPosH.y >= 1.000 || shadowPosH.z <= 0 || shadowPosH.z >= 1.000)
//     return 1;
//     float depth = shadowPosH.z;
    
//     uint width, height, numLevels;
//     globalPointShadowMap.GetDimensions(0, width, height, numLevels);

//     float dx = 1.0f / (float)width;
//     float dy = 1.0f / (float)height;

//     float2 offset[9] = 
//     {
//         float2(-dx, -dy), float2(0, -dy), float2(dx, -dy),
//         float2(-dx, 0), float2(0, 0), float2(dx, 0),
//         float2(-dx, dy), float2(0, dy), float2(dx, dy)
//     };

//     float persentage = 0;
//     [unroll]
//     for(int i = 0; i < 9; ++i)
//     {
//         //persentage += globalPointShadowMap.SampleCmpLevelZero(globalComSampler, shadowPosH.xy + offset[i], depth).r;
//     }

//     return persentage / 9.0f;
// }
float ComputePointShadowFactor(float3 posW, float3 lightPos)
{
    float3 fragToLightDir = posW - lightPos;
    float depth = globalPointShadowMap.Sample(globalSampler, fragToLightDir).r;

    depth *= 8000;

    float currentDepth = length(fragToLightDir);

    float bias = 0.05;
    float shadow = currentDepth - bias > depth ? 0.0f : 1.0f;
    return shadow;
}

#endif