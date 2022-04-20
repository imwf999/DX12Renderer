#pragma once
#include "ConstValue.h"
#include <d3d12.h>
#include <wrl.h>

namespace rdr
{
	using Microsoft::WRL::ComPtr;

	class PipelineStateObject
	{
	public:
		PipelineStateObject() = default;
		PipelineStateObject(D3D12_GRAPHICS_PIPELINE_STATE_DESC InDesc) : pipelineStateDesc(InDesc) {  }
		~PipelineStateObject();

	public:
		void SetInputLayout(const D3D12_INPUT_LAYOUT_DESC& InputLayout);
		void SetRootSignature(ID3D12RootSignature* ptrRootSignature);
		void SetVertexShader(const D3D12_SHADER_BYTECODE& shaderCode);
		void SetPixelShader(const D3D12_SHADER_BYTECODE& pixelShader);
		void SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerDesc);
		void SetBlendState(const D3D12_BLEND_DESC& blendState);
		void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc);
		void SetNumRenderTargets(UINT number);
		void SetRTVFormat(uint32_t index, DXGI_FORMAT format);
		void SetDSVFormat(DXGI_FORMAT format);
		void Create(ID3D12Device4* device);
		ID3D12PipelineState* GetPSO() const { return pPipelineState.Get(); }

	public:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc;
		ComPtr<ID3D12PipelineState> pPipelineState;
	};
}
