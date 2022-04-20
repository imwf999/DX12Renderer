#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <any>

namespace rdr
{
	class Material;
	class Renderer;

	class MaterialPool
	{
	public:
		MaterialPool(const Renderer& InRenderer);
		~MaterialPool();

		std::shared_ptr<Material> AddMaterial(const std::string& InName, const std::string& InShaderName, const std::vector<std::pair<std::string, std::any>>& InCBufferData);
		std::shared_ptr<Material> AddMaterialFromSponzaMesh(const std::string& meshName);
		const std::shared_ptr<Material>& GetMaterial(const std::string& InName);

	private:
		std::unordered_map<std::string, std::shared_ptr<Material>> MaterialData;
		const Renderer& renderer;
	};

}
