#include "Texture.h"
#include "DescriptorPool.h"
#include "Renderer.h"

namespace rdr
{
	void Texture::CreateSRV(const Renderer& renderer, DXGI_FORMAT format, uint32_t mipLevel, bool demensionIs2D)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = format;
		srvDesc.ViewDimension = demensionIs2D == true ? D3D12_SRV_DIMENSION_TEXTURE2D : D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MipLevels = mipLevel;

		srvIndex = renderer.GetDescPool()->CreateSRV(this->pResource.Get(), &srvDesc);
	}
}
