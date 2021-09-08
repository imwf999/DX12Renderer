#ifndef RDR_TEXTUREPOOL_H
#define RDR_TEXTUREPOOL_H
#include <memory>
#include <vector>
#include <unordered_map>
#include <Windows.h>
#include "DepthTexture.h"
namespace rdr
{
	class Texture;
	class ITexture;
	class RendererFacade;

	class TexturePool
	{
	public:
		TexturePool(const RendererFacade& renderer);
		~TexturePool();

		Texture* GetDiffuseTex(const std::string& name) const
		{
			auto it = diffuseTexMap.find(name);
			if (it != diffuseTexMap.end())
				return it->second.get();
			return defaultDiffuseTex.get();
		}

		Texture* GetNormalTex(const std::string& name) const
		{
			auto it = normalTexMap.find(name);
			if (it != normalTexMap.end())
				return it->second.get();
			return defaultNormalTex.get();
		}

		Texture* GetDefaultDiffTex() const { return defaultDiffuseTex.get(); }
		Texture* GetDefaultNorTex() const { return defaultNormalTex.get(); }
		size_t GetDiffTexNum() const { return diffuseTexMap.size(); }
		size_t GetNorTexNum() const { return normalTexMap.size(); }
		ID3D12Resource* GetMainShadowMap() const { return depthTexVec[1]->GetDefaultResource(); }
		ID3D12Resource* GetPointShadowMap() const { return depthTexVec[2]->GetDefaultResource(); }

	private:
		std::vector<std::unique_ptr<Texture>> cubeMapVec;
		std::unordered_map<std::string, std::unique_ptr<Texture>> diffuseTexMap;
		std::unordered_map<std::string, std::unique_ptr<Texture>> normalTexMap;
		std::vector < std::unique_ptr<DepthTexture>> depthTexVec;
		std::unique_ptr<Texture> defaultDiffuseTex;
		std::unique_ptr<Texture> defaultNormalTex;
	};
}
#endif
