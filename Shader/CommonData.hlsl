cbuffer CameraData : register(b0)
{
    float4x4 globalView;
    float4x4 globalProj;
    float4x4 globalViewProj;                                //这个是摄像机视角下的
    float4x4 globalViewProjForShadow;           //这个是用在阴影贴图那趟pass中的矩阵，这里的viewproj是光源视角下的
    float4x4 globalProjTex;
    float4x4 globalInverseProj;
    float4x4 globalShadowMVP;
    float4 globalEyePostion;
};