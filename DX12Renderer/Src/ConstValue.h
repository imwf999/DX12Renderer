#pragma once
#include <stdint.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

namespace rdr
{
#define SSAO 0			//SSAO开关，置为0关掉SSAO，PhongShader.hlsl中的SSAO宏也要置为0，加了SSR之后这个暂时不用。
#define BACKGROUND_CLEAR_COLOR DirectX::Colors::LightSteelBlue
#define PHONG_SHADER_FILE_PATH L"..\\Shader\\PhongShader.hlsl"
#define SHADOW_MAP_SHADER_FILE_PATH L"..\\Shader\\ShadowMap.hlsl"
#define SKYBOX_SHADER_FILE_PATH L"..\\Shader\\SkyBox.hlsl"
#define GBUFFER_SHADER_FILE_PATH L"..\\Shader\\DrawGBuffer.hlsl"
#define SSAO_SHADER_FILE_PATH L"..\\Shader\\SSAO.hlsl"
#define SSAOBLUR_SHADER_FILE_PATH L"..\\Shader\\SSAOBlur.hlsl"
#define SSR_SHADER_FILE_PATH L"..\\Shader\\SSR.hlsl"
#define AUDIO_PATH "Sound"

	//窗口宽度
	constexpr int global_WindowWidth = 1280;
	//窗口高度
	constexpr int global_WindowHeight = 800;
	//阴影图宽度
	constexpr int global_ShadowMapWidth = global_WindowWidth;
	//阴影图高度
	constexpr int global_ShadowMapHeight = global_WindowHeight;
	//初始时窗口离左边宽度
	constexpr int global_WindowLeft = 100;
	//初始时窗口离顶部宽度
	constexpr int global_WindowTop = 5;
	//初始描述符堆大小
	constexpr int global_InitDescHeapLength = 65;
	//RTV描述符堆初始长度
	constexpr int global_RTVHeapLength = 10;
	//交换链中后台缓冲区个数
	constexpr int global_SwapChainBufferCount = 2;
	//摄像机移动速度，慢速
	constexpr float global_cameraMoveLowSpeed = 300.0f;
	//摄像机移动速度，快速
	constexpr float global_cameraMoveHighSpeed = 700.0f;
	//生成阴影的正交投影的宽度
	constexpr uint32_t global_lightProjectionWidth = 5000;
	//生成阴影图的正交投影的高度
	constexpr uint32_t global_lightProjectionHeight = 5000;
	//初始默认堆大小
	constexpr uint64_t global_DefaultHeapSize = D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT * 120;
	//初始上传堆大小
	constexpr uint64_t global_UploadHeapSize = D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT * 150;

	constexpr  uint32_t global_SSAOSampleCount = 14;

	constexpr int global_SSAOMapWidth = static_cast<int>(global_WindowWidth * 0.5);

	constexpr int global_SSAOMapHeight = static_cast<int>(global_WindowHeight * 0.5);

	constexpr int global_SSAORandomVecMapWidth = global_SSAOMapWidth;

	constexpr int global_SSAORandomVecMapHeight = global_SSAOMapHeight;
}
