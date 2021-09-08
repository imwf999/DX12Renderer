//#include "RenderTarget.h"
//#include "Renderer.h"
//#include "DescriptorPool.h"
//#include <d3d12.h>
//#include "RendererHelper.h"
//#include "d3dx12.h"
//#include "DDSTextureLoader12.h"
//#include "HeapResourceManager.h"
//#include <DirectXColors.h>
//namespace rdr {
//
//void RenderTarget::InitializeSrv(const Renderer& renderer)
//{
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MipLevels = pDefaultTexture->GetDesc().MipLevels;
//	renderer.GetDescriptorPool()->CreateSRV(pDefaultTexture.Get(), &srvDesc);
//	this->indexInSrv = renderer.GetDescriptorPool()->GetCurrentSrvIndex();
//}
//
//void RenderTarget::InitializeRtv(const Renderer& renderer)
//{
//	renderer.GetDescriptorPool()->CreateRTV(pDefaultTexture.Get(), nullptr);
//}
//
//void RenderTarget::Initialize(const std::wstring& path, const Renderer& renderer)
//{
//	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
//		DXGI_FORMAT_R8G8B8A8_UNORM,
//		global_WindowWidth,
//		global_WindowHeight,
//		1,
//		1,
//		1,
//		0,
//		 D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
//
//	this->LoadResourceToGPU(renderer, desc);
//	InitializeSrv(renderer);
//	InitializeRtv(renderer);
//}
//
//void RenderTarget::LoadResourceToGPU(const Renderer& renderer, const D3D12_RESOURCE_DESC& resourceDesc)
//{
//	D3D12_CLEAR_VALUE stClear = {};
//	stClear.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	const float tempArr[4] = { Colors::LightSteelBlue.f[0], Colors::LightSteelBlue.f[1], Colors::LightSteelBlue.f[2], Colors::LightSteelBlue.f[3] };
//	memcpy(&stClear.Color, &tempArr, 4 * sizeof(float));
//
//	renderer.GetHeapResData()->CreateResourceInDefaultHeap(
//		renderer.GetHeapResData()->GetDefaultHeapOffset(),
//		&resourceDesc,
//		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
//		&stClear,
//		pDefaultTexture);
//}
//
//}

#include "RenderTexture.h"
#include "RendererFacade.h"
namespace rdr
{
	RenderTexture::RenderTexture(const RendererFacade& renderer, DXGI_FORMAT format, uint32_t width, uint32_t height, unsigned int type)
	{
		//TODO:
	}
}