#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <memory>

#include "RendererHelper.h"

namespace rdr
{
	class Shader;

	class ShaderPool
	{
	public:
		ShaderPool(ID3D12Device4* device);
		~ShaderPool();

	public:
		Shader& AddShader(const std::string& name);
		std::shared_ptr<Shader> GetShader(const std::string& name) const
		{
			auto index = ShaderMap.find(name);
			if (index == ShaderMap.end())
				DX_THROW("no such shader");
			return ShaderMap.at(name);
		}

	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderMap;
	};
}
