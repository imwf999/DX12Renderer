﻿#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "RendererHelper.h"
namespace rdr
{
	using Microsoft::WRL::ComPtr;

	class Commond
	{
	public:
		Commond(ID3D12Device4* pd3dDevice);
		~Commond();

	public:
		void WaitForGPU()
		{
			++fenceValue;
			ThrowIfFailed(pCommandQueue->Signal(pFence.Get(), fenceValue));
			if (pFence->GetCompletedValue() < fenceValue)
			{
				ThrowIfFailed(pFence->SetEventOnCompletion(fenceValue, fenceEvent));
				WaitForSingleObject(fenceEvent, INFINITE);
			}
		}

		void Reset(ID3D12PipelineState* pso)
		{
			ThrowIfFailed(pCommandAlloc->Reset());
			ThrowIfFailed(pCommandList->Reset(pCommandAlloc.Get(), pso));
		}

		ID3D12GraphicsCommandList* GetCmdList() const { return pCommandList.Get(); }
		ID3D12CommandQueue* GetCmdQueue() const { return pCommandQueue.Get(); }
	private:
		ComPtr<ID3D12CommandQueue> pCommandQueue;
		ComPtr<ID3D12CommandAllocator> pCommandAlloc;
		ComPtr<ID3D12GraphicsCommandList> pCommandList;
		ComPtr<ID3D12Fence> pFence;
		UINT64 fenceValue;
		HANDLE fenceEvent;
	};
}