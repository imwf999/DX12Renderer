#ifndef RDR_DESCRIPTORPOOL_H
#define RDR_DESCRIPTORPOOL_H
#include <unordered_map>
#include <memory>
#include <d3d12.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

namespace rdr
{
	class DescriptorHandle
	{
	public:
		DescriptorHandle() = default;
		DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
		{
			this->cpuHandle = cpuHandle;
			this->gpuHandle = gpuHandle;
		}
		~DescriptorHandle() = default;

	public:
		const D3D12_GPU_DESCRIPTOR_HANDLE& GpuHandle() const { return gpuHandle; }
		const D3D12_CPU_DESCRIPTOR_HANDLE& CpuHandle() const { return cpuHandle; }
		D3D12_GPU_DESCRIPTOR_HANDLE& GpuHandle() { return gpuHandle; }
		D3D12_CPU_DESCRIPTOR_HANDLE& CpuHandle() { return cpuHandle; }

		DescriptorHandle& Offset(uint32_t descriptorNum, uint32_t descriptorSize)
		{
			gpuHandle.ptr += descriptorNum * descriptorSize;
			cpuHandle.ptr += descriptorNum * descriptorSize;
			return *this;
		}

		DescriptorHandle& Offset(uint32_t descriptorSize)
		{
			gpuHandle.ptr += descriptorSize;
			cpuHandle.ptr += descriptorSize;
			return *this;
		}

	private:
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	};

	struct DescriptorHeap
	{
		DescriptorHeap(ID3D12Device* pDevice, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type, bool isShaderVisible)
			: heapCapacity(numDescriptors), currentDescIndex(0)
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.NumDescriptors = numDescriptors;
			desc.Type = type;
			desc.Flags = (isShaderVisible == true ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
			desc.NodeMask = 0;
			pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(pDescriptorHeap.GetAddressOf()));

			this->descriptorSize = pDevice->GetDescriptorHandleIncrementSize(type);
		}
		~DescriptorHeap() = default;

		ComPtr<ID3D12DescriptorHeap> pDescriptorHeap;			//指向GPU描述符堆
		UINT descriptorSize;							//单个描述符长度
		UINT heapCapacity;									//描述符堆容量
		UINT currentDescIndex;					//指向当前描述符的位置
	};

	class DescriptorPool
	{
	public:
		struct DescriptorPoolInitializeDesc;
	public:
		DescriptorPool(Microsoft::WRL::ComPtr<ID3D12Device4> device, DescriptorPoolInitializeDesc desc);
		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator= (const DescriptorPool&) = delete;
		~DescriptorPool();

	public:
		uint32_t CreateSRV(ID3D12Resource* pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc);
		uint32_t CreateUAV();	//TODO:
		uint32_t CreateRTV(ID3D12Resource* pResource, const D3D12_RENDER_TARGET_VIEW_DESC* desc);
		uint32_t CreateDSV(ID3D12Resource* pResource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc);
		uint32_t CreateSampler(const D3D12_SAMPLER_DESC* desc);

	public:
		void SetDescriptorHeap(ID3D12GraphicsCommandList* list)
		{
			ID3D12DescriptorHeap* descHeap[] =
			{
				pSrvUavCbvHeap->pDescriptorHeap.Get(),
				pSamplerHeap->pDescriptorHeap.Get()
			};
			list->SetDescriptorHeaps(_countof(descHeap), descHeap);
		}
		ID3D12DescriptorHeap* GetSrvCbvUavDescriptorHeap() const { return pSrvUavCbvHeap->pDescriptorHeap.Get(); }
		ID3D12DescriptorHeap* GetSamplerDescriptorHeap() const { return pSamplerHeap->pDescriptorHeap.Get(); }
		DescriptorHandle GetSrvStart() const { return srvStart; }
		//DescriptorHandle GetUavStart() const { return uavStart; }
		DescriptorHandle GetRtvStart() const { return rtvStart; }
		DescriptorHandle GetDsvStart() const { return dsvStart; }
		DescriptorHandle GetSamplerStart() const { return samplerStart; }
		uint32_t GetSamplerSize() const { return pSamplerHeap->descriptorSize; }
		uint32_t GetRtvDescSize() const { return pRtvHeap->descriptorSize; }
		uint32_t GetDsvDescSize() const { return pDsvHeap->descriptorSize; }
		uint32_t GetSrvCbvUavDescSize() const { return pSrvUavCbvHeap->descriptorSize; }

	public:
		struct DescriptorPoolInitializeDesc		//用于初始化设定所有的描述符堆是否shader可见
		{
			bool isSrvUavCbvShaderVisible = true;
			bool isRtvShaderVisible = false;
			bool isDsvShaderVisible = false;
			bool isSamplerVisible = true;
		};

	private:
		std::unique_ptr<DescriptorHeap> pSrvUavCbvHeap;
		std::unique_ptr<DescriptorHeap> pDsvHeap;
		std::unique_ptr<DescriptorHeap> pSamplerHeap;
		std::unique_ptr<DescriptorHeap> pRtvHeap;

		DescriptorHandle srvStart;
		//DescriptorHandle uavStart;
		DescriptorHandle dsvStart;
		DescriptorHandle rtvStart;
		DescriptorHandle samplerStart;

		uint32_t srvNum;
		uint32_t uavNum;

		ComPtr<ID3D12Device4> pDevice;
	};
}

#endif
