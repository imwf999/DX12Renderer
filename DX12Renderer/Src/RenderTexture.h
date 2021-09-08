#ifndef RDR_RENDERTEXTURE_H
#define RDR_RENDERTEXTURE_H
#include "ConstValue.h"
#include "ITexture.h"
namespace rdr
{
	class RendererFacade;

	class RenderTexture : public ITexture
	{
	public:
		RenderTexture() = default;
		RenderTexture(const RendererFacade& renderer, DXGI_FORMAT format, uint32_t width, uint32_t height, unsigned int type);
		virtual ~RenderTexture() = default;

	public:
		uint32_t GetRtvIndex() const { return rtvIndex; }

	private:
		uint32_t rtvIndex;
	};
}
#endif
