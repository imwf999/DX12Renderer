#define MaxLightCount 10
//#define POINTSHADOW
struct Light
{
    float3 lightColor;
    float fallOffStart;
    float4 direction;
    float3 position;
    float fallOffEnd;
};

Texture2D globalTextureArr[40] : register(t0, space0);

TextureCube globalCubeMap : register(t0, space2);

Texture2D globalShadowMap : register(t1, space2);
#ifdef POINTSHADOW
//Texture2D globalPointShadowMapArr[6] : register(t2, space2);
TextureCube globalPointShadowMap : register(t2, space2);
#endif
SamplerState globalSampler : register(s0);
SamplerComparisonState globalComSampler : register(s1);

cbuffer ObjConstBuffer : register(b0)
{
    float4x4 globalWorldMatrix;
    float4x4 globalNormalWorldMatrix;
};

cbuffer PassConstBuffer : register(b1)
{
    float4x4 globalViewProj;
    float4x4 globalShadowMVP;
    #ifdef POINTSHADOW
    float4x4 globalPointShadowMVPRight;
    float4x4 globalPointShadowMVPLeft;
    float4x4 globalPointShadowMVPForward;
    float4x4 globalPointShadowMVPBack;
    float4x4 globalPointShadowMVPUp;
    float4x4 globalPointShadowMVPDown;
    #endif
    float3 globalEyePostion;
    uint globalPointLightNum;
    Light globalLights[MaxLightCount];
};

cbuffer Material : register(b2)
{
    float4 globalDiffuse;
    float4 globalAmbient;
    float4 globalSpecular;
    float4 globalEmissive;
    float4 globalTransparent;
    float4 globalReflective;
    float3 globalFresnelR0;
    float globalShininess;
    float globalBumpScaling;
    float globaRreflectivity;
    uint globalDiffuseIndex;
    uint globalNormalIndex;
};