#include "CommonFunction.hlsl"

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
    float3 worldPos : POSITION0;
    float4 shadowPosH : POSITION1;
    #ifdef POINTSHADOW
    float4 shadowRightPosH : POSITIONT2;
    float4 shadowLeftPosH : POSITIONT3;
    float4 shadowForwardPosH : POSITIONT4;
    float4 shadowBackPosH : POSITIONT5;
    float4 shadowUpPosH : POSITIONT6;
    float4 shadowDownPosH : POSITIONT7;
    #endif
    float2 uv : TEXCOORD;
    float3 worldNormal : NORMAL;
    float3 worldTangent : TANGENT;
};

VertexOutput VSMain(VertexInput vin)
{
    VertexOutput vout;

    vout.worldPos = mul(float4(vin.pos, 1), globalWorldMatrix);
    vout.worldNormal = mul(vin.normal, (float3x3)globalNormalWorldMatrix);
    vout.worldTangent = mul( vin.tangent, (float3x3)globalWorldMatrix);
    vout.pos = mul(float4(vout.worldPos, 1), globalViewProj);

    vout.uv = vin.uv;
    vout.shadowPosH = mul(float4(vin.pos, 1), globalShadowMVP);
#ifdef POINTSHADOW
    vout.shadowRightPosH = mul(float4(vin.pos, 1), globalPointShadowMVPRight);
    vout.shadowLeftPosH = mul(float4(vin.pos, 1), globalPointShadowMVPLeft);
    vout.shadowForwardPosH = mul(float4(vin.pos, 1), globalPointShadowMVPForward);
    vout.shadowBackPosH = mul(float4(vin.pos, 1), globalPointShadowMVPBack);
    vout.shadowUpPosH = mul(float4(vin.pos, 1), globalPointShadowMVPUp);
    vout.shadowDownPosH = mul(float4(vin.pos, 1), globalPointShadowMVPDown);
#endif
    return vout;
}

float4 PSMain(VertexOutput pin) : SV_TARGET
{

    pin.worldNormal = normalize(pin.worldNormal);

    pin.worldTangent = normalize(pin.worldTangent);
    float3 worldBinormal = cross(pin.worldNormal, pin.worldTangent);

    float3x3 worldTBN = float3x3(pin.worldTangent, worldBinormal, pin.worldNormal);

    float4 tempNormal = globalTextureArr[globalNormalIndex].Sample(globalSampler, pin.uv);
    tempNormal = 2.0f * tempNormal - 1.0f;

    float3 worldNormal = mul(tempNormal.rgb, worldTBN);

    float shadowFactor = 1.0f;
    shadowFactor = CalculateShadowFactor(pin.shadowPosH);

    float4 diffuseAbedo = globalTextureArr[globalDiffuseIndex].Sample(globalSampler, pin.uv) * globalDiffuse;

    float3 ambient = globalAmbient.rgb * diffuseAbedo.rgb;               //环境光颜色

    float m = globalShininess * 256 * tempNormal.a; 

    float3 directionalLight = (0, 0, 0);            //平行光贡献的光亮
    float3 pointLight = (0, 0, 0);                  //点光源贡献的光亮

#ifdef POINTSHADOW
    for(int i = 1; i <= globalPointLightNum - 1; ++i)
    {
        pointLight += ComputePointLight(globalLights[i], pin.worldPos, worldNormal, m, diffuseAbedo);
    }

    float3 mainPointLight = ComputePointLight(globalLights[globalPointLightNum], pin.worldPos, worldNormal, m, diffuseAbedo);

    float mainPointShadowFactor = ComputePointShadowFactor(pin.worldPos, globalLights[globalPointLightNum].position);

    mainPointLight *= mainPointShadowFactor;

    pointLight += mainPointLight;
#else
    directionalLight = ComputeDirectionalLight(globalLights[0], pin.worldPos, worldNormal, m, diffuseAbedo);

    directionalLight *= shadowFactor;
    for(int i = 1; i <= globalPointLightNum; ++i)
    {
        pointLight += ComputePointLight(globalLights[i], pin.worldPos, worldNormal, m, diffuseAbedo);
    }
#endif
    float3 toalColor = ambient + pointLight + directionalLight;
    float4 finalColor = float4(toalColor, diffuseAbedo.a);

    return finalColor;
}