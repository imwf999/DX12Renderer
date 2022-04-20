#include "HeapManager.h"
#include "RendererHelper.h"
#include "d3dx12.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"

namespace rdr
{
	HeapManager::HeapManager(ComPtr<ID3D12Device4>& pDevice, UINT64 defaultHeapSize, UINT64 uploadHeapSize)
		: defaultHeapOffset(0), uploadHeapOffset(0)
	{
		this->pDevice = pDevice;

		D3D12_HEAP_DESC defaultHeapDesc = {};
		defaultHeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;			//如果要开MSAA就需要4M对齐
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

	HeapManager::~HeapManager()
	{

	}

	void HeapManager::CreateBufferInDefaultHeap(const void* pData, BaseResource* ptrResource, ID3D12GraphicsCommandList* pList)
	{
		ThrowIfFailed(pDevice->CreatePlacedResource(
			pDefaultHeap.Get(),
			defaultHeapOffset,
			&ptrResource->GetResourceDesc(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(ptrResource->GetDefaultResourceAddressOf())));

		defaultHeapOffset = UpperAlignment(defaultHeapOffset + ptrResource->GetResourceDesc().Width, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

		ThrowIfFailed(pDevice->CreatePlacedResource(
			pUploadHeap.Get(),
			uploadHeapOffset,
			&ptrResource->GetResourceDesc(),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(ptrResource->GetIntermediateResourceAddressOf())));

		uploadHeapOffset = UpperAlignment(uploadHeapOffset + ptrResource->GetResourceDesc().Width, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

		D3D12_SUBRESOURCE_DATA subresource = {};
		subresource.pData = pData;
		subresource.RowPitch = ptrResource->GetResourceDesc().Width;
		subresource.SlicePitch = subresource.RowPitch;
		std::vector<D3D12_SUBRESOURCE_DATA> resourceVec;
		resourceVec.push_back(subresource);

		UpdateSubresources(pList, ptrResource->GetDefaultResource(), ptrResource->GetIntermediateResource(), 0, 0, 1, resourceVec.data());

		auto indexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			ptrResource->GetDefaultResource(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ);

		pList->ResourceBarrier(1, &indexBarrier);
	}

	void rdr::HeapManager::CreateTextureInDefaultHeap(Texture* pResource, ID3D12GraphicsCommandList* pList, std::vector<D3D12_SUBRESOURCE_DATA>& subResourceVec)
	{
		ThrowIfFailed(pDevice->CreatePlacedResource(
			pDefaultHeap.Get(),
			defaultHeapOffset,
			&pResource->GetResourceDesc(),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(pResource->GetDefaultResourceAddressOf())));

		uint32_t numSubresource = pResource->GetResourceDesc().DepthOrArraySize * pResource->GetResourceDesc().MipLevels;

		//之前一直是把这个值乘以2再作为后面上传堆的大小，可能之前是其他地方弄错了误以为是这里上传堆的大小不够
		uint64_t textureUploadSize = GetRequiredIntermediateSize(pResource->GetDefaultResource(), 0, numSubresource);
		CD3DX12_RESOURCE_DESC resourceUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(textureUploadSize);

		this->defaultHeapOffset = UpperAlignment(defaultHeapOffset + static_cast<uint64_t>(textureUploadSize * OffsetRate), D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

		ThrowIfFailed(pDevice->CreatePlacedResource(
			pUploadHeap.Get(),
			uploadHeapOffset,
			&resourceUploadDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(pResource->GetIntermediateResourceAddressOf())));

		this->uploadHeapOffset = UpperAlignment(uploadHeapOffset + textureUploadSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

		auto tempBarrier1 = CD3DX12_RESOURCE_BARRIER::Transition(
			pResource->GetDefaultResource(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST);

		pList->ResourceBarrier(1, &tempBarrier1);

		UpdateSubresources(pList, pResource->GetDefaultResource(), pResource->GetIntermediateResource(), 0, 0, numSubresource, subResourceVec.data());

		auto tempBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			pResource->GetDefaultResource(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		pList->ResourceBarrier(1, &tempBarrier);
	}

	void HeapManager::CreateTextureInDefaultHeap(Texture* pResource, ID3D12GraphicsCommandList* pList)
	{
		ThrowIfFailed(pDevice->CreatePlacedResource(
			pDefaultHeap.Get(),
			defaultHeapOffset,
			&pResource->GetResourceDesc(),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(pResource->GetDefaultResourceAddressOf())));

		uint32_t numSubresource = pResource->GetResourceDesc().DepthOrArraySize * pResource->GetResourceDesc().MipLevels;
		uint64_t textureUploadSize = GetRequiredIntermediateSize(pResource->GetDefaultResource(), 0, numSubresource);
		this->defaultHeapOffset = UpperAlignment(defaultHeapOffset + static_cast<uint64_t>(textureUploadSize * OffsetRate), D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
	}

	void HeapManager::CreateBufferInUploadHeap(BaseResource* pResource, uint32_t bufferSize)
	{
		ThrowIfFailed(pDevice->CreatePlacedResource(
			this->pUploadHeap.Get(),
			uploadHeapOffset,
			&pResource->GetResourceDesc(),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(pResource->GetDefaultResourceAddressOf())));

		uploadHeapOffset = UpperAlignment(uploadHeapOffset + bufferSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
	}
}