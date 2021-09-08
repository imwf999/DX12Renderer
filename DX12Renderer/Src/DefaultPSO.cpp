#include "DefaultPSO.h"
#include "RootSignature.h"
#include "Pass.h"
namespace rdr {

void DefaultPSO::Create(const Pass& pass, ID3D12Device4* device)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};
	pipelineStateDesc.InputLayout = { inputElementVec.data(), static_cast<uint32_t>(inputElementVec.size()) };
	pipelineStateDesc.pRootSignature = pass.GetRootSignature();
	pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(pass.GetVertexShaderCode(), pass.GetVertexShaderSize());
	pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(pass.GetPixelShaderCode(), pass.GetPixelShaderSize());
	pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.NumRenderTargets = 1;
	pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineStateDesc.SampleDesc.Quality = 0;

	ThrowIfFailed(device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(this->pPipelineState.GetAddressOf())));
}

}