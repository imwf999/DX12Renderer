#include "ShadowMapPSO.h"
#include "RootSignature.h"
#include "Pass.h"
namespace rdr
{
	void ShadowMapPSO::Create(const Pass& pass, ID3D12Device4* device)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC smapPsoDesc = {};
		smapPsoDesc.InputLayout = { inputElementVec.data(), static_cast<UINT>(inputElementVec.size()) };
		smapPsoDesc.pRootSignature = pass.GetRootSignature();
		smapPsoDesc.VS = CD3DX12_SHADER_BYTECODE(pass.GetVertexShaderCode(), pass.GetVertexShaderSize());
		smapPsoDesc.PS = CD3DX12_SHADER_BYTECODE(pass.GetPixelShaderCode(), pass.GetPixelShaderSize());
		smapPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
#ifdef POINTSHADOW
		smapPsoDesc.RasterizerState.DepthBias = 10;
#else
		smapPsoDesc.RasterizerState.DepthBias = 100000;
#endif
		smapPsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
		smapPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
		smapPsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		smapPsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		smapPsoDesc.SampleMask = UINT_MAX;
		smapPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		smapPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		smapPsoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		smapPsoDesc.NumRenderTargets = 0;
		smapPsoDesc.SampleDesc.Count = 1;
		smapPsoDesc.SampleDesc.Quality = 0;

		ThrowIfFailed(device->CreateGraphicsPipelineState(&smapPsoDesc, IID_PPV_ARGS(this->pPipelineState.GetAddressOf())));
	}
}