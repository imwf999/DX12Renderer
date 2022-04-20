#pragma once
#include <string>
#include "ConstValue.h"
#include "Texture.h"

namespace rdr
{
	class Renderer;

	class RenderTexture : public Texture
	{
	public:
		RenderTexture(const std::string& InName) { name = InName; }
		RenderTexture(const std::string& InName, const Renderer& renderer, DXGI_FORMAT format, uint32_t width = global_WindowWidth, uint32_t height = global_WindowHeight);
		~RenderTexture() override = default;

	public:
		uint32_t GetRtvIndex() const { return rtvIndex; }
		void SetStateFromPresentToRenderTarget(ID3D12GraphicsCommandList* pList) const;
		void SetStateFromRenderTargetToPresent(ID3D12GraphicsCommandList* pList) const;
		void SetStateFromRenderTargetToShaderResource(ID3D12GraphicsCommandList* pList) const;
		void SetStateFromShaderResourceToRenderTarget(ID3D12GraphicsCommandList* pList) const;

	private:
		void CreateRTV(const Renderer& renderer, DXGI_FORMAT format);

	public:
		static constexpr DXGI_FORMAT ScreenNormalMapFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
		static constexpr DXGI_FORMAT SsaoMapFormat = DXGI_FORMAT_R16_UNORM;

	private:
		uint32_t rtvIndex = 0;
	};
}
