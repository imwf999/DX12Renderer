#include "DepthTexture.h"

#include "Commond.h"
#include "d3dx12.h"
#include "HeapManager.h"
#include "Renderer.h"
#include "DescriptorPool.h"

namespace rdr
{
	DepthTexture::DepthTexture(const std::string& InName)
		: dsvIndex(0)
	{
		name = InName;
	}

	void DepthTexture::Create2D(const Renderer& renderer, DXGI_FORMAT format, uint32_t width, uint32_t height)
	{
		SetResourceDescAsTexture(format, width, height, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		renderer.GetResMgr()->CreateTextureInDefaultHeap(this, renderer.GetCommond()->GetCmdList());

		//当深度纹理的资源类型是format时，需要创建的srv类型就是format的枚举项加一的那一项，目前只用到了D32_FLOAT和D24_S8也就是普通深度纹理和阴
		//影纹理两种，目前是这样，未来加入其他类型的深度纹理时可能还会变
		CreateDSV(renderer, format);
		CreateSRV(renderer, static_cast<DXGI_FORMAT>(format + 1));
	}

	void DepthTexture::CreateDSV(const Renderer& renderer, DXGI_FORMAT format)
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
		viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Format = format;
		viewDesc.Flags = D3D12_DSV_FLAG_NONE;
		viewDesc.Texture2D.MipSlice = 0;
		this->dsvIndex = renderer.GetDescPool()->CreateDSV(pResource.Get(), &viewDesc);
	}

	void DepthTexture::SetStateFromDepthWriteToGenericRead(ID3D12GraphicsCommandList* pList) const
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			pResource.Get(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_RESOURCE_STATE_GENERIC_READ);
		pList->ResourceBarrier(1, &barrier);
	}

	DepthTexture::~DepthTexture() = default;
}
