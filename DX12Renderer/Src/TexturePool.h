#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <Windows.h>
#include "DepthTexture.h"
#include "ConstValue.h"

namespace rdr
{
	class Texture;
	class CommonTexture;
	class Renderer;
	class RenderTexture;

	using TexSharedPtr = std::shared_ptr<Texture>;

	class TexturePool
	{
	public:
		TexturePool(const Renderer& renderer);
		~TexturePool();

		const TexSharedPtr& GetTexture(const std::string& name) const
		{
			const auto& index = TextureMap.find(name);
			if (index == TextureMap.end())
				throw "No Such Texture";
			return index->second;
		}

		void AddTexture(const TexSharedPtr& ptrTex)
		{
			const std::string& tName = ptrTex->GetName();
			const auto& tIndex = TextureMap.find(tName);
			if (tIndex != TextureMap.cend())
				throw "Two Texture Have The Same Name";
			TextureMap.insert({ tName, ptrTex });
		}

		bool ContainTexture(const std::string& name)
		{
			auto index = TextureMap.find(name);
			return index != TextureMap.end();
		}

	private:
		std::unordered_map<std::string, TexSharedPtr> TextureMap;
	};
}