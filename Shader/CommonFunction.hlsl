#include "CommonData.hlsl"
//Note:
//在透视投影的情况下，执行了MVP变换后，坐标位于投影空间，应该先进行透视除法使xy位于-1到1之间，如果要作为纹理采样
//的坐标，再执行[-1,1]->[0,1]的纹理变换(纹理变换描述位于龙书597页)，也就是先乘以0.5再加上0.5(这里只考虑x，y的话是乘以-0.5，因为坐标系倒过
//来了)，开始以为这个顺序应该是不能变的，也就是要先执行透视除法再执行纹理变换，但是龙书上的例子全部都是先执行纹理变
//换再执行透视除法，结果也没有问题，调试发现其实这两种顺序结果是完全一样的，如果先执行纹理变换，也就是
//MVP之后就执行“x * 0.5 + 0.5”，但是放在矩阵里面算的话其实是“x * 0.5 + 0.5 * w”，这样再执行透视除法除以w，得到
//(x / w) * 0.5 + 0.5，如果先执行透视除法，(x / w, y / w, z / w, w / w) -> (x', y', z', 1) 再执行纹理变换
//得到x' * 0.5 + 0.5 * 1 -> (x / w) * 0.5 + 0.5，这和上面结果完全一样，所以顺序怎么样都行，而且和透视投影还是正交投影
//也没有关系，正交投影的MVP变换之后就已经在NDC空间了，w值为1。上面是把纹理变换写成矩阵形式，如果是写成表达式的话
//就需要注意加的是0.5w而不是0.5，龙书上是把Texture变换和MVP变换在CPU端已经乘在了一起然后作为一个矩阵传进来，然后在GPU端
//执行透视除法，而如果要把Texture变换和MVP变换分开并且把它和透视除法都放在在GPU端执行的话，就需要注意这里是加0.5*w而不是0.5
//另外还要注意手动进行透视除法要把整个向量除以w而不能仅仅把xyz除以w
//下面这个函数函数用于执行纹理变化，传进来坐标有可能是投影空间下也可能是NDC空间下，也就是有可能是已经执行了透视除法
//也可能没有执行，所以这里要写成加0.5 * w而不是0.5
float4 TextureTranform(float4 Pos)
{
    return float4(Pos.x * 0.5f + 0.5f * Pos.w, 0.5f * Pos.w - Pos.y * 0.5f, Pos.z, Pos.w);
}

//上述变换的逆变换，把坐标从纹理空间变换回之前的空间
float4 InverseTextureTransform(float4 texPos)
{
    return float4(texPos.x * 2.0f - 1.0f * texPos.w, 1.0f * texPos.w - texPos.y * 2.0f, texPos.z, texPos.w);
}

float NdcDepthToViewDepth(float z_ndc)
{
    float viewZ = globalProj[3][2] / (z_ndc - globalProj[2][2]);
    return viewZ;
}

float3 ComputeDirectLight(float3 lightColor, float3 lightDir, float3 normal)
{
    return lightColor * max(0, dot(lightDir, normal));
}

float3 ComputeDiffuse(float3 lightColor, float3 lightDir, float3 normal, float4 diffuseAbedo)
{
    return ComputeDirectLight(lightColor, lightDir, normal) * diffuseAbedo.rgb;
}

float3 ComputeSpecular(float3 lightColor, float3 lightDir, float3 posW, float3 normal, float m, float3 fresnelR0)
{
    float3 viewDir = normalize(posW - globalEyePostion.xyz);

    float3 fresnelRF = fresnelR0 + (1 - fresnelR0) * pow(1 - saturate(dot(lightDir, normal)), 5);

    float3 halfDir = normalize(lightDir + viewDir); //直射入眼睛的那部分光线和人眼之间的微平面法线
    float roughnessFactor = (m + 8) * pow(max(dot(normal, halfDir), 0), m) / 8;

    float3 specAbedo = fresnelRF * roughnessFactor;
    specAbedo = specAbedo / (specAbedo + 1.0f); //有些高光因子会大于1，如果没有这个处理的话有些像素会呈现纯白色
    return ComputeDirectLight(lightColor, lightDir, normal) * specAbedo;
}

//点光源暂时没用
//float3 ComputePointLight(Light pointLight, float3 posW, float3 normal, float m, float4 diffuseAbedo, float3 fresnelR0)
//{
//    float3 lightDir = pointLight.position - posW;
//    float lightDistance = length(lightDir);
//    if (lightDistance > pointLight.fallOffEnd)
//        return 0.0f;
//    float fallFactor = saturate((pointLight.fallOffEnd - lightDistance) / (pointLight.fallOffEnd - pointLight.fallOffStart));

//    lightDir /= lightDistance;
//    float3 diffuse = ComputeDiffuse(pointLight.lightColor, lightDir, normal, diffuseAbedo);
//    float3 specular = ComputeSpecular(pointLight.lightColor, lightDir, posW, normal, m, fresnelR0);

//    return (diffuse + specular) * fallFactor;
//}

//函数内部把方向取反，所以外部传入的方向是从光源指向目标点
float3 ComputeDirectionalLight(float3 lightDir, float3 lightColor, float3 posW, float3 normal, float m, float4 diffuseAbedo, float3 fresnelR0)
{
    lightDir = -lightDir;
    float3 diffuse = ComputeDiffuse(lightColor, lightDir, normal, diffuseAbedo);
    float3 specular = ComputeSpecular(lightColor, lightDir, posW, normal, m, fresnelR0);
    return diffuse + specular;
}