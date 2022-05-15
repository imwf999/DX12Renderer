#include "DescriptorPool.h"
#include "ConstValue.h"
#include "RendererHelper.h"
#include "d3dx12.h"

namespace rdr
{
	DescriptorPool::DescriptorPool(ComPtr<ID3D12Device4> device, DescriptorPoolInitializeDesc desc)
		: pDevice(device), srvNum(0), uavNum(0)
	{
		//TODO:描述符堆中描述符的数量，这里没有把参数暴露出来，直接写个默认值进去，要考虑更改一下

		pSrvUavCbvHeap = std::make_unique<DescriptorHeap>(
			device.Get(),
			global_InitDescHeapLength,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			desc.isSrvUavCbvShaderVisible);

		pDsvHeap = std::make_unique<DescriptorHeap>(
			device.Get(),
			global_InitDescHeapLength,
			D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			desc.isDsvShaderVisible);

		pRtvHeap = std::make_unique<DescriptorHeap>(
			device.Get(),
			global_RTVHeapLength,
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			desc.isRtvShaderVisible);

		pSamplerHeap = std::make_unique<DescriptorHeap>(
			device.Get(),
			global_InitDescHeapLength,
			D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
			desc.isSamplerVisible);

		srvStart = 
		{ 
			pSrvUavCbvHeap->pDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 
			pSrvUavCbvHeap->pDescriptorHeap->GetGPUDescriptorHandleForHeapStart() 
		};
		dsvStart = 
		{ 
			pDsvHeap->pDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			pDsvHeap->pDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
		};
		rtvStart =
		{ 
			pRtvHeap->pDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 
			pRtvHeap->pDescriptorHeap->GetGPUDescriptorHandleForHeapStart() 
		};
		samplerStart =
		{
			pSamplerHeap->pDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			pSamplerHeap->pDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
		};

		CreateSamplers();
	}

	DescriptorPool::~DescriptorPool()
	{
	}

	uint32_t DescriptorPool::CreateSRV(ID3D12Resource* pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc)
	{
		if (srvNum >= global_InitDescHeapLength)
			DX_THROW("descriptor pool over flow");
		D3D12_CPU_DESCRIPTOR_HANDLE handle = pSrvUavCbvHeap->pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += static_cast<UINT64>(srvNum) * pSrvUavCbvHeap->descriptorSize;
		pDevice->CreateShaderResourceView(pResource, desc, handle);
		return srvNum++;
	}

	uint32_t DescriptorPool::CreateRTV(ID3D12Resource* pResource, const D3D12_RENDER_TARGET_VIEW_DESC* desc)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
			pRtvHeap->pDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			pRtvHeap->currentDescIndex,
			pRtvHeap->descriptorSize);
		pDevice->CreateRenderTargetView(pResource, desc, handle);
		return pRtvHeap->currentDescIndex++;
	}

	uint32_t DescriptorPool::CreateDSV(ID3D12Resource* pResource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
			pDsvHeap->pDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			pDsvHeap->currentDescIndex,
			pDsvHeap->descriptorSize);
		pDevice->CreateDepthStencilView(pResource, desc, handle);
		return pDsvHeap->currentDescIndex++;
	}

	uint32_t DescriptorPool::CreateSampler(const D3D12_SAMPLER_DESC* desc)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
			pSamplerHeap->pDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			pSamplerHeap->currentDescIndex,
			pSamplerHeap->descriptorSize);
		pDevice->CreateSampler(desc, handle);
		return pSamplerHeap->currentDescIndex++;
	}

	void DescriptorPool::CreateSamplers()
	{
		D3D12_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.MaxAnisotropy = 8;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		this->CreateSampler(&samplerDesc);

		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		samplerDesc.BorderColor[0] = 0.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 0.0f;
		samplerDesc.BorderColor[3] = 1.0f;
		this->CreateSampler(&samplerDesc);

		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		this->CreateSampler(&samplerDesc);

		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.MaxAnisotropy = 0;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 1.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;
		this->CreateSampler(&samplerDesc);
	}
}
