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

		std::shared_ptr<Material> AddMaterial(const std::string& InName, const std::string& InShaderName);
		std::vector<std::shared_ptr<Material>> AddMaterialFromSponzaMesh(const std::string& meshName);
		const std::shared_ptr<Material>& GetMaterial(const std::string& InName);
		void UpdateMaterialData();
		void SetOpenIndirectLight(bool isOpen) { CacheOpenIndirectLight = isOpen; DataIsDirty = true; }

	private:
		std::unordered_map<std::string, std::shared_ptr<Material>> MaterialData;
		const Renderer& renderer;
		bool CacheOpenIndirectLight = false;
		bool DataIsDirty = false;
	};

}
