#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <string>
#include <memory>
#include <assimp/Importer.hpp>
#include "ConstValue.h"

using Microsoft::WRL::ComPtr;
struct aiScene;
struct aiMesh;

namespace rdr
{
	class Renderer;
	class VertexBuffer;
	class IndexBuffer;
	class Material;
	class DescriptorPool;
	class FrameConstBuffer;
	struct VertexData;

	class Mesh
	{
	public:
		Mesh();
		Mesh(const Renderer& renderer, uint32_t InVertexNum, const std::string& InMatName, uint32_t InStartIndexLocation, uint32_t InBaseVertexLocation);
		Mesh(Mesh&& other) noexcept;
		~Mesh() = default;

	private:
		struct Submesh
		{
			std::shared_ptr<Material> material;
			union
			{
				uint32_t indexCount;
				uint32_t vertexCount;
			};
			uint32_t startIndexLocation;
			uint32_t baseVertexLocation;
		};

	public:
		void DrawByIndex(ID3D12GraphicsCommandList* pList, const DescriptorPool* pDescriptorPool, const FrameConstBuffer& frameBuffer, const Material& material);
		void DrawByIndex(ID3D12GraphicsCommandList* pList, const DescriptorPool* pDescriptorPool, const FrameConstBuffer& frameBuffer);
		void LoadSponza(const std::string& filePath, const Renderer& renderer);
		void LoadSkyBox(const std::string& filePath, const Renderer& renderer);
		const std::vector<Submesh>& GetSubmeshData() const { return submeshVec; }

	private:
		void LoadSponza(const std::string& filePath, const Renderer& renderer, std::vector<VertexData>& vertexVec, std::vector<uint32_t>& indexVec);
		void AddSponzaSubmesh(const aiMesh& mesh, const Renderer& renderer, uint32_t indexOffset, uint32_t vertexOffset);

	private:
		std::unique_ptr<VertexBuffer> pVertexBuffer;
		std::unique_ptr<IndexBuffer> pIndexBuffer;
		std::vector<Submesh> submeshVec;

	private:
		const aiScene* pModel;
		std::unique_ptr<Assimp::Importer> ptrAiImportor;
	};
}
