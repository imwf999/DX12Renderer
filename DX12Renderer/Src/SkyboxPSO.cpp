#include "SkyboxPSO.h"
#include "RootSignature.h"
#include "Pass.h"
namespace rdr 
{
	void SkyboxPSO::Create(const Pass& pass, ID3D12Device4* device)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = { inputElementVec.data(), static_cast<uint32_t>(inputElementVec.size()) };
		desc.pRootSignature = pass.GetRootSignature();
		desc.VS = CD3DX12_SHADER_BYTECODE(pass.GetVertexShaderCode(), pass.GetVertexShaderSize());
		desc.PS = CD3DX12_SHADER_BYTECODE(pass.GetPixelShaderCode(), pass.GetPixelShaderSize());
		desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		ThrowIfFailed(device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(this->pPipelineState.GetAddressOf())));
	}
}