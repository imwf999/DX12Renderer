#include "HeapResourceManager.h"
#include "RendererHelper.h"
#include "d3dx12.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace rdr
{
	HeapResourceManager::HeapResourceManager(ComPtr<ID3D12Device4>& pDevice, UINT64 defaultHeapSize, UINT64 uploadHeapSize)
		: defaultHeapOffset(0), uploadHeapOffset(0)
	{
		this->pDevice = pDevice;

		D3D12_HEAP_DESC defaultHeapDesc = {};
		defaultHeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;			//TODO:如果要开MSAA就需要4M对齐
		defaultHeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		defaultHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		defaultHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		defaultHeapDesc.SizeInBytes = defaultHeapSize;
		defaultHeapDesc.Flags = D3D12_HEAP_FLAG_NONE;
		ThrowIfFailed(pDevice->CreateHeap(&defaultHeapDesc, IID_PPV_ARGS(pDefaultHeap.GetAddressOf())));

		D3D12_HEAP_DESC uploadHeapDesc = {};
		uploadHeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		uploadHeapDesc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
		uploadHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		uploadHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadHeapDesc.SizeInBytes = uploadHeapSize;
		uploadHeapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
		ThrowIfFailed(pDevice->CreateHeap(&uploadHeapDesc, IID_PPV_ARGS(pUploadHeap.GetAddressOf())));
	}

	HeapResourceManager::~HeapResourceManager()
	{

	}

	void HeapResourceManager::CreateBufferResourceInDefaultHeap(
		void* pData,
		uint32_t bufferSize,
		const D3D12_CLEAR_VALUE* value,
		ComPtr<ID3D12Resource>& pResource,
		ComPtr<ID3D12Resource>& pintermidiateRes,
		ID3D12GraphicsCommandList* pList)
	{
		auto indexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

		ThrowIfFailed(pDevice->CreatePlacedResource(
			pDefaultHeap.Get(),
			defaultHeapOffset,
			&indexBufferDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			value,
			IID_PPV_ARGS(pResource.GetAddressOf())));

		defaultHeapOffset = UpperAlignment(defaultHeapOffset + bufferSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

		ThrowIfFailed(pDevice->CreatePlacedResource(
			pUploadHeap.Get(),
			uploadHeapOffset,
			&indexBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(pintermidiateRes.GetAddressOf())));

		uploadHeapOffset = UpperAlignment(uploadHeapOffset + bufferSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

		D3D12_SUBRESOURCE_DATA subresource = {};
		subresource.pData = pData;
		subresource.RowPitch = bufferSize;
		subresource.SlicePitch = subresource.RowPitch;

		UpdateSubresources<1>(pList, pResource.Get(), pintermidiateRes.Get(), 0, 0, 1, &subresource);

		auto indexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			pResource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ);

		pList->ResourceBarrier(1, &indexBarrier);
	}

	void rdr::HeapResourceManager::CreateTexResourceInDefaultHeap(
		Texture* pResource,
		D3D12_RESOURCE_STATES InitialState,
		const D3D12_CLEAR_VALUE* pOptimizedClearValue,
		ID3D12GraphicsCommandList* pList)
	{
		ThrowIfFailed(pDevice->CreatePlacedResource(
			pDefaultHeap.Get(),
			defaultHeapOffset,
			&pResource->resourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			pOptimizedClearValue,
			IID_PPV_ARGS(pResource->pResource.GetAddressOf())));

		uint32_t numSubresource = pResource->resourceDesc.DepthOrArraySize * pResource->resourceDesc.MipLevels;

		//DOTO:这里用的方法是把资源大小乘以2计算偏移量，目前可以正常工作。后期需要优化
		auto textureSize = GetRequiredIntermediateSize(pResource->pResource.Get(), 0, numSubresource);
		auto textureUploadSize = textureSize * 2;
		auto resourceUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(textureUploadSize);

		this->defaultHeapOffset = UpperAlignment(defaultHeapOffset + textureUploadSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

		ThrowIfFailed(pDevice->CreatePlacedResource(
			pUploadHeap.Get(),
			uploadHeapOffset,
			&resourceUploadDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(pResource->pIntermediateResource.GetAddressOf())));

		this->uploadHeapOffset = UpperAlignment(uploadHeapOffset + textureUploadSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

		auto tempBarrier1 = CD3DX12_RESOURCE_BARRIER::Transition(
			pResource->pResource.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST);

		pList->ResourceBarrier(1, &tempBarrier1);

		UpdateSubresources(pList, pResource->pResource.Get(), pResource->pIntermediateResource.Get(), 0, 0, numSubresource, pResource->subResourceVec.data());

		auto tempBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			pResource->pResource.Get(), 
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		pList->ResourceBarrier(1, &tempBarrier);
	}

	void HeapResourceManager::CreateCommittedResource(
		const D3D12_HEAP_PROPERTIES* pHeapProperties,
		D3D12_HEAP_FLAGS HeapFlags,
		const D3D12_RESOURCE_DESC* pDesc,
		D3D12_RESOURCE_STATES InitialResourceState,
		const D3D12_CLEAR_VALUE* pOptimizedClearValue,
		ID3D12Resource** pResource)
	{
		ThrowIfFailed(pDevice->CreateCommittedResource(
			pHeapProperties, 
			HeapFlags, 
			pDesc,
			InitialResourceState, 
			pOptimizedClearValue,
			IID_PPV_ARGS(pResource)));
	}

	void HeapResourceManager::CreateBufferResourceInUploadHeap(
		const D3D12_RESOURCE_DESC* pDesc, 
		D3D12_RESOURCE_STATES InitialState, 
		const D3D12_CLEAR_VALUE* pOptimizedClearValue, 
		ComPtr<ID3D12Resource>& pUploadRes,
		uint32_t bufferSize)
	{
		ThrowIfFailed(pDevice->CreatePlacedResource(
			this->pUploadHeap.Get(),
			uploadHeapOffset,
			pDesc,
			InitialState,
			pOptimizedClearValue,
			IID_PPV_ARGS(pUploadRes.GetAddressOf())));

		uploadHeapOffset = UpperAlignment(uploadHeapOffset + bufferSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
	}
}