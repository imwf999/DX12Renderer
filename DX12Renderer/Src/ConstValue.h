#ifndef RDR_CONSTVALUE_H
#define RDR_CONSTVALUE_H
#include <stdint.h>
#include <d3d12.h>
#include <DirectXMath.h>
namespace rdr
{
	//点光源的宏，取消注释后会关闭平行光开启中央点光源，并投射阴影，运行没问题，但是点光阴影的投射
	// 还没完全做好，另外CommonData.hlsl中对应的同名的宏也要取消注释
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
	constexpr float global_cameraMoveHighSpeed = 700.0f;
	//生成阴影的正交投影的宽度
	constexpr uint32_t global_lightProjectionWidth = 5000;
	//生成阴影图的正交投影的高度
	constexpr uint32_t global_lightProjectionHeight = 5000;
	//初始默认堆大小
	constexpr uint64_t global_DefaultHeapSize = D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT * 120;
	//初始上传堆大小
	constexpr uint64_t global_UploadHeapSize = D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT * 160;
}

#endif
