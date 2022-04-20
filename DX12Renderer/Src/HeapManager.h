#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <cstdint>
#include <vector>
using Microsoft::WRL::ComPtr;

namespace rdr
{
	class Texture;
	class BaseResource;

	class HeapManager
	{
	public:
		HeapManager(ComPtr<ID3D12Device4>& pDevice, UINT64 defaultHeapSize, UINT64 uploadHeapSize);
		HeapManager(const HeapManager&) = delete;
		HeapManager& operator= (const HeapManager&) = delete;
		~HeapManager();

	public:
		void CreateTextureInDefaultHeap(Texture* pResource, ID3D12GraphicsCommandList* pList, std::vector<D3D12_SUBRESOURCE_DATA>& subResourceVec);
		void CreateTextureInDefaultHeap(Texture* pResource, ID3D12GraphicsCommandList* pList);
		void CreateBufferInDefaultHeap(const void* pData, BaseResource* ptrResource, ID3D12GraphicsCommandList* pList);
		void CreateBufferInUploadHeap(BaseResource* pResource, uint32_t bufferSize);

	private:
		ComPtr<ID3D12Heap> pDefaultHeap;
		ComPtr<ID3D12Heap> pUploadHeap;
		ComPtr<ID3D12Device> pDevice;

	private:
		UINT64 uploadHeapOffset;
		UINT64 defaultHeapOffset;

	private:
		//GetRequiredIntermediateSize函数得到的是上传堆需要的空间，如果直接把这个值作为默认堆向前的偏移值会出现一些创建的纹理有重叠
		//所以需要一个倍率，在GetRequiredIntermediateSize返回值的基础上乘以这个倍率，再作为defaultHeapOffset向前的偏移值。
		//目前定为1.1没有问题，后面加入的纹理种类多了之后可能还会出问题，还需要调整
		static constexpr float OffsetRate = 1.1f;
	};
}
