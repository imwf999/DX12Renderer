#ifndef RDR_CONSTVALUE_H
#define RDR_CONSTVALUE_H
#include <stdint.h>
#include <d3d12.h>
#include <DirectXMath.h>
namespace rdr
{
//#define POINTSHADOW
	//窗口宽度
	constexpr int global_WindowWidth = 1280;
	//窗口高度
	constexpr int global_WindowHeight = 800;
	//初始时窗口离左边宽度
	constexpr int global_WindowLeft = 100;
	//初始时窗口离顶部宽度
	constexpr int global_WindowTop = 5;
	//初始描述符堆大小
	constexpr int global_InitDescHeapLength = 50;
	//交换链中前后台缓冲区个数
	constexpr int global_SwapChainBufferNum = 5;
	//摄像机移动速度，慢速
	constexpr float global_cameraMoveLowSpeed = 300.0f;
	//摄像机移动速度，快速
	constexpr float global_cameraMoveHighSpeed = 670.0f;
	//生成阴影的正交投影的宽度
	constexpr uint32_t global_lightProjectionWidth = 5000;
	//生成阴影图的正交投影的高度
	constexpr uint32_t global_lightProjectionHeight = 5000;
	//平行光方向	
	constexpr DirectX::XMFLOAT4 global_directionalLightDirection = { 0.0f, -0.8f, 0.3f, 0 };
	//主点光源位置
	constexpr DirectX::XMFLOAT3 global_mainPointPosition = { 0, 600, 0 };
	//点光源的几种颜色
	constexpr DirectX::XMFLOAT3 global_ColorRed = { 1, 0, 0 };
	constexpr DirectX::XMFLOAT3 global_ColorGreen = { 0, 1, 0 };
	constexpr DirectX::XMFLOAT3 global_ColorBlue = { 0, 0, 1 };
	constexpr DirectX::XMFLOAT3 global_ColorPurple = { 0.5f, 0, 0.5f };
	constexpr DirectX::XMFLOAT3 global_ColorYellow = { 1, 1, 0 };
	constexpr DirectX::XMFLOAT3 global_ColorWhite = { 1, 1, 1 };
	//初始默认堆大小
	constexpr uint64_t global_DefaultHeapSize = D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT * 120;
	//初始上传堆大小
	constexpr uint64_t global_UploadHeapSize = D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT * 160;
}

#endif
