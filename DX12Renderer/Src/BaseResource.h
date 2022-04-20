#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>

using Microsoft::WRL::ComPtr;

namespace rdr
{
	class BaseResource
	{
	public:
		BaseResource() = default;
		virtual ~BaseResource() = default;

	public:
		D3D12_GPU_VIRTUAL_ADDRESS GetVirtualAddress() const { return pResource->GetGPUVirtualAddress(); }
		ID3D12Resource* GetDefaultResource() const { return pResource.Get(); }
		ID3D12Resource** GetDefaultResourceAddressOf() { return pResource.GetAddressOf(); }
		ID3D12Resource* GetIntermediateResource() const { return pIntermediateResource.Get(); }
		ID3D12Resource** GetIntermediateResourceAddressOf() { return pIntermediateResource.GetAddressOf(); }
		const D3D12_RESOURCE_DESC& GetResourceDesc() const { return resourceDesc; }

	protected:
		void SetResourceDescAsBuffer( uint32_t width);
		void SetResourceDescAsTexture(DXGI_FORMAT format, uint32_t width, uint32_t height, D3D12_RESOURCE_FLAGS flags);

	protected:
		ComPtr<ID3D12Resource> pResource;
		ComPtr<ID3D12Resource> pIntermediateResource;
		D3D12_RESOURCE_DESC resourceDesc = {};
	};
}