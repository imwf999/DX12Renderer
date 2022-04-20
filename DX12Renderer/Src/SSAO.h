#pragma once
#include <memory>
#include <d3d12.h>

#include "ConstValue.h"

namespace rdr
{
	class RenderTexture;
	class Renderer;
	class DescriptorPool;
	class Material;
	class Commond;

	class SSAOMap
	{
		using RenderTexturePtr = std::shared_ptr<RenderTexture>;

	public:
		SSAOMap(const Renderer& renderer);
		~SSAOMap() = default;

	public:
		void Draw(uint32_t blurCount = 3);
		const RenderTexturePtr& GetSSAOMap() const { return ptrSSAOMap; }
		const RenderTexturePtr& GetSSAOBlurMap() const { return ptrSSAOBlurMap; }

	private:
		void DrawSSAOMap();
		void HorizontalBlur(ID3D12GraphicsCommandList* pList);
		void VerticalBlur(ID3D12GraphicsCommandList* pList);
		void DrawInstance(const Material& material, const RenderTexture& renderTarget);

	private:
		std::shared_ptr<Material> ptrSSAOMaterial;
		std::shared_ptr<Material> ptrSSAOHBlurMaterial;
		std::shared_ptr<Material> ptrSSAOVBlurMaterial;
		const Renderer* ptrRenderer;
		RenderTexturePtr ptrSSAOMap;		//这个既作为SSAO图，也作为偶数次模糊的渲染目标
		RenderTexturePtr ptrSSAOBlurMap;
		D3D12_VIEWPORT ViewPort = { 0, 0, static_cast<float>(global_SSAOMapWidth), static_cast<float>(global_SSAOMapHeight), 0, 1.0f };
		D3D12_RECT Rect = { 0, 0,static_cast<LONG>(global_SSAOMapWidth), static_cast<LONG>(global_SSAOMapHeight) };
	};
}
