#pragma once
#include "ConstValue.h"
#include "RendererHelper.h"
#include "d3dx12.h"
#include <d3d12.h>
#include <vector>
#include <functional>
namespace rdr
{
	using Microsoft::WRL::ComPtr;
	class RootSignature
	{
	public:
		RootSignature(size_t numRootParam, size_t numStaticSampler = 0)
		{
			rootParameterVec.resize(numRootParam);
			samplers.resize(numStaticSampler);
		}
		~RootSignature() = default;

	public:
		void Create(ID3D12Device* pDevice, const std::function<void()>& deletePtrFunc);
		ID3D12RootSignature* GetSignature() { return pRootSignature.Get(); }
		size_t GetParameterCount() const { return rootParameterVec.size(); }
		CD3DX12_ROOT_PARAMETER1& operator[] (size_t index)
		{
			return rootParameterVec[index];
		}

	private:
		std::vector<CD3DX12_ROOT_PARAMETER1> rootParameterVec;
		std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;
		ComPtr<ID3D12RootSignature> pRootSignature;
	};
}