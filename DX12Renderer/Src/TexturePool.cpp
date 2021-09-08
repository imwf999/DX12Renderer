#include "TexturePool.h"
#include "Texture.h"
#include "RendererFacade.h"
#include "DepthTexture.h"
#include "RendererHelper.h"
#include <filesystem>

namespace rdr
{
	TexturePool::TexturePool(const RendererFacade& renderer)
	{
		std::string path = "Texture\\pbr";
		auto result = std::filesystem::directory_iterator(path);
		std::wstring texturePath;
		for (auto& entry : result)
		{
			texturePath = entry.path();
			auto dotLoc = texturePath.find_last_of(L'.');
			if(texturePath.substr(dotLoc + 1, texturePath.size() - dotLoc - 1) != L"dds")
				continue;
			std::unique_ptr<Texture> tex = std::make_unique<Texture>(texturePath, renderer, true);
			size_t begin = texturePath.find_last_of('\\');
			size_t end = texturePath.find_last_of('_');
			std::string name = WstrToStr(texturePath.substr(begin + 1, end - begin - 1));
			if (texturePath[dotLoc - 1] == L'e')
				diffuseTexMap[name] = std::move(tex);
			else if (texturePath[dotLoc - 1] == L'l')
				normalTexMap[name] = std::move(tex);
		}
		defaultDiffuseTex = std::make_unique<Texture>(L"Texture\\default\\default_diffuse(2).dds", renderer, true);
		defaultNormalTex = std::make_unique<Texture>(L"Texture\\default\\default_normal(2).dds", renderer, true);
		std::unique_ptr<Texture> pSkybox = std::make_unique<Texture>(L"Texture\\cubemap.dds", renderer, false);
		cubeMapVec.push_back(std::move(pSkybox));

		std::unique_ptr<DepthTexture> depthTex = std::make_unique<DepthTexture>(renderer);
		depthTexVec.push_back(std::move(depthTex));

		std::unique_ptr<DepthTexture> shadowMap = std::make_unique<DepthTexture>(
			renderer,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			global_WindowWidth,
			global_WindowHeight,
			true);
		depthTexVec.push_back(std::move(shadowMap));

#ifdef POINTSHADOW
			std::unique_ptr<DepthTexture> pointShadowMap = std::make_unique<DepthTexture>(
				renderer,
				DXGI_FORMAT_D24_UNORM_S8_UINT,
				global_WindowWidth,
				global_WindowHeight,
				false);
			depthTexVec.push_back(std::move(pointShadowMap));
#endif
	}

	TexturePool::~TexturePool()
	{
	}
}