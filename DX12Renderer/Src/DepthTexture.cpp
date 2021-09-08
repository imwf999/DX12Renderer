#include "DepthTexture.h"
#include "d3dx12.h"
#include "HeapResourceManager.h"
#include "RendererFacade.h"
#include "DescriptorPool.h"
namespace rdr
{
	//DepthTexture::DepthTexture(const RendererFacade& renderer, DXGI_FORMAT format, uint32_t width, uint32_t height, unsigned int type)
	//{
	//	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
	//		format, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	//	D3D12_CLEAR_VALUE value = {};
	//	value.Format = format;
	//	value.DepthStencil.Depth = 1.0f;
	//	value.DepthStencil.Stencil = 0;
	//	auto tempHeapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	//	renderer.GetResMgr()->CreateCommittedResource(
	//		&tempHeapPro,
	//		D3D12_HEAP_FLAG_NONE,
	//		&desc,
	//		D3D12_RESOURCE_STATE_DEPTH_WRITE,
	//		&value,
	//		pResource.GetAddressOf());
	//	D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
	//	viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	//	viewDesc.Format = format;
	//	viewDesc.Flags = D3D12_DSV_FLAG_NONE;
	//	viewDesc.Texture2D.MipSlice = 0;
	//	this->dsvIndex = renderer.GetDescPool()->CreateDSV(pResource.Get(), &viewDesc);
	//	if ((type & 0x1) == 1)
	//	{
	//		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	//		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	//		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//		srvDesc.Texture2D.MostDetailedMip = 0;
	//		srvDesc.Texture2D.MipLevels = 1;
	//		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	//		srvDesc.Texture2D.PlaneSlice = 0;
	//		this->srvIndex = renderer.GetDescPool()->CreateSRV(pResource.Get(), &srvDesc);
	//	}
	//}

	DepthTexture::DepthTexture(const RendererFacade& renderer)
	{
		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
			DefaultFormat, global_WindowWidth, global_WindowHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		D3D12_CLEAR_VALUE value = {};
		value.Format = DefaultFormat;
		value.DepthStencil.Depth = 1.0f;
		value.DepthStencil.Stencil = 0;
		auto tempHeapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		renderer.GetResMgr()->CreateCommittedResource(
			&tempHeapPro,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&value,
			pResource.GetAddressOf());
		D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
		viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Format = DefaultFormat;
		viewDesc.Flags = D3D12_DSV_FLAG_NONE;
		viewDesc.Texture2D.MipSlice = 0;
		this->dsvIndex = renderer.GetDescPool()->CreateDSV(pResource.Get(), &viewDesc);
	}

	DepthTexture::DepthTexture(const RendererFacade& renderer, DXGI_FORMAT format, uint32_t width, uint32_t height, bool is2D)
	{
		if (is2D == true)
			Create2DDepthTexture(renderer, format, width, height);
		else
			CreateCubeDepthTexture(renderer, format, width, height);
	}

	void DepthTexture::Create2DDepthTexture(const RendererFacade& renderer, DXGI_FORMAT format, uint32_t width, uint32_t height)
	{
		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
			format, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		D3D12_CLEAR_VALUE value = {};
		value.Format = format;
		value.DepthStencil.Depth = 1.0f;
		value.DepthStencil.Stencil = 0;

		auto tempHeapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		renderer.GetResMgr()->CreateCommittedResource(
			&tempHeapPro,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&value,
			pResource.GetAddressOf());

		D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
		viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Format = format;
		viewDesc.Flags = D3D12_DSV_FLAG_NONE;
		viewDesc.Texture2D.MipSlice = 0;
		this->dsvIndex = renderer.GetDescPool()->CreateDSV(pResource.Get(), &viewDesc);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		srvDesc.Texture2D.PlaneSlice = 0;
		this->srvIndex = renderer.GetDescPool()->CreateSRV(pResource.Get(), &srvDesc);
	}

	void DepthTexture::CreateCubeDepthTexture(const RendererFacade& renderer, DXGI_FORMAT format, uint32_t width, uint32_t height)
	{
		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
			format, width, height, 6, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		D3D12_CLEAR_VALUE value = {};
		value.Format = format;
		value.DepthStencil.Depth = 1.0f;
		value.DepthStencil.Stencil = 0;

		auto tempHeapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		renderer.GetResMgr()->CreateCommittedResource(
			&tempHeapPro,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&value,
			pResource.GetAddressOf());

		//D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
		//viewDesc.Format = format;
		//viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
		//viewDesc.Texture2DArray.FirstArraySlice = 0;
		//viewDesc.Texture2DArray.ArraySize = 6;
		//viewDesc.Texture2DArray.MipSlice = 0;
		//viewDesc.Flags = D3D12_DSV_FLAG_NONE;
		//this->dsvIndex = renderer.GetDescPool()->CreateDSV(pResource.Get(), &viewDesc);

		for (int i = 0; i < 6; ++i)
		{
			D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
			viewDesc.Format = format;
			viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
			viewDesc.Texture2DArray.MipSlice = 0;
			viewDesc.Texture2DArray.ArraySize = 1;
			viewDesc.Texture2DArray.FirstArraySlice = i;
			viewDesc.Flags = D3D12_DSV_FLAG_NONE;
			//TODO:这个变量要改
			this->dsvIndex = renderer.GetDescPool()->CreateDSV(pResource.Get(), &viewDesc);
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = 1;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		this->srvIndex = renderer.GetDescPool()->CreateSRV(pResource.Get(), &srvDesc);
	}

	DepthTexture::~DepthTexture()
	{
	}
}