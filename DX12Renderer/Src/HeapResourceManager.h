#ifndef RDR_HEAPRESOURCEDATA_H
#define RDR_HEAPRESOURCEDATA_H
#include <wrl.h>
#include <d3d12.h>
#include <stdint.h>
#include <vector>
using Microsoft::WRL::ComPtr;

namespace rdr
{
	class Texture;
	class IResource;
	class VertexBuffer;
	class IndexBuffer;

	class HeapResourceManager
	{
	public:
		HeapResourceManager(ComPtr<ID3D12Device4>& pDevice, UINT64 defaultHeapSize, UINT64 uploadHeapSize);
		HeapResourceManager(const HeapResourceManager&) = delete;
		HeapResourceManager& operator= (const HeapResourceManager&) = delete;
		~HeapResourceManager();

	public:
		void CreateTexResourceInDefaultHeap(
			Texture* pResource,
			D3D12_RESOURCE_STATES InitialState,
			const D3D12_CLEAR_VALUE* pOptimizedClearValue,
			ID3D12GraphicsCommandList* pList);

		void CreateBufferResourceInDefaultHeap(
			void* pData,
			uint32_t bufferSize,
			const D3D12_CLEAR_VALUE* value,
			ComPtr<ID3D12Resource>& pResource,
			ComPtr<ID3D12Resource>& pintermidiateRes,
			ID3D12GraphicsCommandList* pList
		);

		void CreateBufferResourceInUploadHeap(
			const D3D12_RESOURCE_DESC* pDesc,
			D3D12_RESOURCE_STATES InitialState,
			const D3D12_CLEAR_VALUE* pOptimizedClearValue,
			ComPtr<ID3D12Resource>& pUploadRes,
			uint32_t bufferSize);

		void CreateCommittedResource(
			const D3D12_HEAP_PROPERTIES* pHeapProperties,
			D3D12_HEAP_FLAGS HeapFlags,
			const D3D12_RESOURCE_DESC* pDesc,
			D3D12_RESOURCE_STATES InitialResourceState,
			const D3D12_CLEAR_VALUE* pOptimizedClearValue,
			ID3D12Resource** pResource);

	private:
		ComPtr<ID3D12Heap> pDefaultHeap;
		ComPtr<ID3D12Heap> pUploadHeap;
		ComPtr<ID3D12Device> pDevice;

	private:
		UINT64 uploadHeapOffset;
		UINT64 defaultHeapOffset;
	};
}

#endif
