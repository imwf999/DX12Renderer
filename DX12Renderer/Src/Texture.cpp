#include "Texture.h"
#include "DescriptorPool.h"
#include "RendererHelper.h"
#include "DDSTextureLoader12.h"
#include "HeapResourceManager.h"
#include "RendererFacade.h"
#include "Display.h"
#include "Commond.h"
using namespace DirectX;

namespace rdr
{
	Texture::Texture(const std::wstring& filePath, const RendererFacade& renderer, bool is2D)
	{
		ThrowIfFailed(LoadDDSTextureFromFile(
			renderer.GetDevice(),
			filePath.c_str(),
			ddsData,
			subResourceVec,
			this->resourceDesc
		));

		renderer.GetResMgr()->CreateTexResourceInDefaultHeap(this, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, renderer.GetCommond()->GetCmdList());

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = resourceDesc.Format;
		srvDesc.ViewDimension = is2D == true ? D3D12_SRV_DIMENSION_TEXTURE2D : D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MipLevels = resourceDesc.MipLevels;

		srvIndex = renderer.GetDescPool()->CreateSRV(this->pResource.Get(), &srvDesc);
	}
}