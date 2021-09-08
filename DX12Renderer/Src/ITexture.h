#pragma once
#include "IResource.h"
namespace rdr
{
	enum TextureType : unsigned int
	{
		type_IsTexture = 0x1,
		type_IsRenderTarget = 0x2,
		type_IsDepthBuffer = 0x4
	};

	class ITexture : public IResource
	{
	public:
		ITexture() = default;
		virtual ~ITexture() = default;

	public:
		uint32_t GetSrvIndex() const { return srvIndex; }

	protected:
		uint32_t srvIndex;
	};
}