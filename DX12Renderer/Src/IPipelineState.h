#pragma once
#include "ConstValue.h"
#include <vector>
#include <d3d12.h>
#include <wrl.h>

namespace rdr
{
	using Microsoft::WRL::ComPtr;
	class Pass;

	class IPipelineState
	{
	public:
		IPipelineState()
		{
			inputElementVec =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};
		}
		virtual ~IPipelineState() = default;

	public:
		virtual void Create(const Pass& pass, ID3D12Device4* device) = 0;
		ID3D12PipelineState* GetPSO() const { return pPipelineState.Get(); }
	protected:
		std::vector< D3D12_INPUT_ELEMENT_DESC> inputElementVec;
		ComPtr<ID3D12PipelineState> pPipelineState;
	};
}