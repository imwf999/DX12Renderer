#pragma once
#include "ITexture.h"

namespace rdr
{
	class RendererFacade;
	class DepthTexture : public ITexture
	{
	public:
		DepthTexture(const RendererFacade& renderer);
		//DepthTexture(const RendererFacade& renderer, DXGI_FORMAT format, uint32_t width, uint32_t height, unsigned int type);
		DepthTexture(const RendererFacade& renderer, DXGI_FORMAT format, uint32_t width, uint32_t height, bool is2D);
		~DepthTexture();

	public:
		uint32_t GetDsvIndex() const { return dsvIndex; }
	private:
		void Create2DDepthTexture(const RendererFacade& renderer, DXGI_FORMAT format, uint32_t width, uint32_t height);
		void CreateCubeDepthTexture(const RendererFacade& renderer, DXGI_FORMAT format, uint32_t width, uint32_t height);
	private:
		uint32_t dsvIndex;
		constexpr static DXGI_FORMAT DefaultFormat = DXGI_FORMAT_D32_FLOAT;
		constexpr static DXGI_FORMAT ShadowFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	};
}