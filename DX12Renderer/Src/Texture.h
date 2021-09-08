#pragma once
#include "ITexture.h"
#include <string>
#include <memory>
#include <vector>
namespace rdr
{
	class HeapResourceManager;
	class RendererFacade;

	class Texture : public ITexture
	{
		friend HeapResourceManager;

	public:
		Texture(const std::wstring& filePath, const RendererFacade& renderer, bool is2D);

	private:
		std::unique_ptr<uint8_t[]> ddsData;
		std::vector<D3D12_SUBRESOURCE_DATA> subResourceVec;
		ComPtr<ID3D12Resource> pIntermediateResource;
	};
}