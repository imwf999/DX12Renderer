#pragma once

#include "d3dx12.h"
#include "RenderTexture.h"
#include "DepthTexture.h"
namespace rdr
{
	struct ShadowMapPSO
	{
		ShadowMapPSO()
		{
			desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			desc.RasterizerState.DepthBias = 100000;
			desc.RasterizerState.DepthBiasClamp = 0.0f;
			desc.RasterizerState.SlopeScaledDepthBias = 1.0f;
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
			desc.DSVFormat = DepthTexture::ShadowFormat;
			desc.NumRenderTargets = 0;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
		}
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {  };
	};

	struct SkyBoxPSO
	{
		SkyBoxPSO()
		{
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
		}
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {  };
	};

	//Note:
	//渲染场景中的叶片时需要把BlendState.AlphaToCoverageEnable置为true，不然边缘部分都是黑色的
	//或者也可以在像素着色器中加一个clip(alpha - 0.01f)，把alpha值等于小于0.01f的剔除
	struct PhongPSO
	{
		PhongPSO()
		{
			desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			//desc.BlendState.AlphaToCoverageEnable = true;
			desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
		}
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {  };
	};

	struct SsaoPSO
	{
		SsaoPSO()
		{
			desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			desc.DepthStencilState.DepthEnable = false;
			desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = RenderTexture::SsaoMapFormat;
			desc.DSVFormat = DXGI_FORMAT_UNKNOWN;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
		}
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {  };
	};

	struct GBufferPSO
	{
		GBufferPSO()
		{
			desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 4;
			desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.RTVFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.RTVFormats[2] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.RTVFormats[3] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
		}
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {  };
	};
}