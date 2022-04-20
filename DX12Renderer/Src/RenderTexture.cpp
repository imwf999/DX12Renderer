#include "RenderTexture.h"
#include "Commond.h"
#include "Renderer.h"
#include "HeapManager.h"
#include "DescriptorPool.h"
#include "ConstValue.h"

namespace rdr
{
	RenderTexture::RenderTexture(const std::string& InName, const Renderer& renderer, DXGI_FORMAT format, uint32_t width, uint32_t height)
	{
		name = InName;

		SetResourceDescAsTexture(format, width, height, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		renderer.GetResMgr()->CreateTextureInDefaultHeap(this,renderer.GetCommond()->GetCmdList());

		CreateRTV(renderer, format);
		CreateSRV(renderer, format);
	}

	void RenderTexture::SetStateFromPresentToRenderTarget(ID3D12GraphicsCommandList* pList) const
	{
		auto tempResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			pResource.Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		pList->ResourceBarrier(1, &tempResourceBarrier);
	}

	void RenderTexture::SetStateFromRenderTargetToPresent(ID3D12GraphicsCommandList* pList) const
	{
		auto tempResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			pResource.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);
		pList->ResourceBarrier(1, &tempResourceBarrier);
	}

	void RenderTexture::SetStateFromRenderTargetToShaderResource(ID3D12GraphicsCommandList* pList) const
	{
		auto tempResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			pResource.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_GENERIC_READ);
		pList->ResourceBarrier(1, &tempResourceBarrier);
	}

	void RenderTexture::SetStateFromShaderResourceToRenderTarget(ID3D12GraphicsCommandList* pList) const
	{
		auto tempResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			pResource.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		pList->ResourceBarrier(1, &tempResourceBarrier);
	}

	void RenderTexture::CreateRTV(const Renderer& renderer, DXGI_FORMAT format)
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Format = format;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;
		this->rtvIndex = renderer.GetDescPool()->CreateRTV(pResource.Get(), &rtvDesc);
	}
}
