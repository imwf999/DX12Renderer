#include "BaseResource.h"

namespace rdr
{
	void BaseResource::SetResourceDescAsBuffer(uint32_t width)
	{
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = width;
		resourceDesc.Height = 1;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
	}

	void BaseResource::SetResourceDescAsTexture(DXGI_FORMAT format, uint32_t width, uint32_t height,
		D3D12_RESOURCE_FLAGS flags)
	{
		this->resourceDesc.Alignment = 0;
		this->resourceDesc.Format = format;
		this->resourceDesc.Width = width;
		this->resourceDesc.Height = height;
		this->resourceDesc.DepthOrArraySize = 1;
		this->resourceDesc.MipLevels = 1;
		this->resourceDesc.SampleDesc.Count = 1;
		this->resourceDesc.SampleDesc.Quality = 0;
		this->resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		this->resourceDesc.Flags = flags;
		this->resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	}
}
