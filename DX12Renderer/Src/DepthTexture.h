#pragma once
#include "Texture.h"
#include "ConstValue.h"

namespace rdr
{
	class Renderer;

	class DepthTexture : public Texture
	{
	public:
		DepthTexture(const std::string& InName);
		~DepthTexture() override;

	public:
		uint32_t GetDsvIndex() const { return dsvIndex; }
		void SetStateFromDepthWriteToGenericRead(ID3D12GraphicsCommandList* pList) const;
		void Create2D(const Renderer& renderer, DXGI_FORMAT format, uint32_t width, uint32_t height);

	public:
		static constexpr DXGI_FORMAT DefaultFormat = DXGI_FORMAT_D32_FLOAT;
		static constexpr DXGI_FORMAT ShadowFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	private:
		void CreateDSV(const Renderer& renderer, DXGI_FORMAT format);		//默认创建2D的DSV

	private:
		uint32_t dsvIndex;
	};
}
