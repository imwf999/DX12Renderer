#include "PSO.h"
#include "d3dUtil.h"

namespace rdr
{
	PipelineStateObject::~PipelineStateObject() = default;

	void PipelineStateObject::SetInputLayout(const D3D12_INPUT_LAYOUT_DESC& InputLayout)
	{
		pipelineStateDesc.InputLayout = InputLayout;
	}

	void PipelineStateObject::SetRootSignature(ID3D12RootSignature* ptrRootSignature)
	{
		pipelineStateDesc.pRootSignature = ptrRootSignature;
	}

	void PipelineStateObject::SetVertexShader(const D3D12_SHADER_BYTECODE& shaderCode)
	{
		pipelineStateDesc.VS = shaderCode;
	}

	void PipelineStateObject::SetPixelShader(const D3D12_SHADER_BYTECODE& pixelShader)
	{
		pipelineStateDesc.PS = pixelShader;
	}

	void PipelineStateObject::SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerDesc)
	{
		pipelineStateDesc.RasterizerState = rasterizerDesc;
	}

	void PipelineStateObject::SetBlendState(const D3D12_BLEND_DESC& blendState)
	{
		pipelineStateDesc.BlendState = blendState;
	}

	void PipelineStateObject::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc)
	{
		pipelineStateDesc.DepthStencilState = depthStencilDesc;
	}

	void PipelineStateObject::SetNumRenderTargets(UINT number)
	{
		pipelineStateDesc.NumRenderTargets = number;
	}

	void PipelineStateObject::SetRTVFormat(uint32_t index, DXGI_FORMAT format)
	{
		if (index >= 8) throw "Index Is Out Of Range";
		pipelineStateDesc.RTVFormats[index] = format;
	}

	void PipelineStateObject::SetDSVFormat(DXGI_FORMAT format)
	{
		pipelineStateDesc.DSVFormat = format;
	}

	void PipelineStateObject::Create(ID3D12Device4* device)
	{
		ThrowIfFailed(device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(this->pPipelineState.GetAddressOf())));
	}
}
