#ifndef RDR_IRESOURCE_H
#define RDR_IRESOURCE_H
#include <d3d12.h>
#include <wrl.h>
#include <stdint.h>
using Microsoft::WRL::ComPtr;
namespace rdr
{
	class IResource
	{
	public:
		IResource() = default;
		virtual ~IResource() = default;

	public:
		D3D12_GPU_VIRTUAL_ADDRESS GetVirtualAddress() const { return pResource->GetGPUVirtualAddress(); }
		ID3D12Resource* GetDefaultResource() const { return pResource.Get(); }
		ID3D12Resource** GetDefaultResourceAddressOf() { return pResource.GetAddressOf(); }
		const D3D12_RESOURCE_DESC& GetResourceDesc() const { return resourceDesc; }

	protected:
		ComPtr<ID3D12Resource> pResource;
		D3D12_GPU_VIRTUAL_ADDRESS virtualAddress;
		D3D12_RESOURCE_DESC resourceDesc;
	};
}
#endif