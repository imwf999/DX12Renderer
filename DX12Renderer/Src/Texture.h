#pragma once
#include <string>
#include "BaseResource.h"

namespace rdr
{
	class Renderer;

	class Texture : public BaseResource
	{
	public:
		Texture() = default;
		~Texture() override = default;

	public:
		uint32_t GetSrvIndex() const { return srvIndex; }
		const std::string& GetName() const { return name; }
		void SetName(const std::string& InName) { name = InName; }

	protected:
		void CreateSRV(const Renderer& renderer, DXGI_FORMAT format, uint32_t mipLevel = 1, bool demensionIs2D = true);

	protected:
		uint32_t srvIndex = 0;
		std::string name;
	};
}
