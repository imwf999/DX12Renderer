#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <string>
#include <memory>
#include <assimp/Importer.hpp>
#include <functional>
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
	class MeshHandler;

	class Mesh
	{
	public:
		Mesh(const Renderer& InRendere);
		Mesh(const Renderer& InRenderer, uint32_t InVertexNum, const std::string& InMatName, uint32_t InStartIndexLocation, uint32_t InBaseVertexLocation);
		Mesh(Mesh&& other) noexcept;
		~Mesh();

	private:
		struct Submesh
		{
			std::vector<std::shared_ptr<Material>> materialVec;
			std::string submeshName;
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
		void DrawByIndex(ID3D12GraphicsCommandList* pList, const DescriptorPool* pDescriptorPool, const FrameConstBuffer& frameBuffer, uint32_t matIndex);
		void LoadSponza(const std::string& filePath);
		void LoadSkyBox(const std::string& filePath);
		const std::vector<Submesh>& GetSubmeshData() const { return submeshVec; }

	private:
		void AddSponzaSubmesh(const aiMesh& mesh, uint32_t indexOffset, uint32_t vertexOffset);
		void AddSponzaSubmesh(const std::string& submeshName, uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset);
		void AddSkyBoxSubmesh(const std::string& submeshName, uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset);
		void LoadMesh(const std::string& filePath, std::vector<VertexData>& vertexVec, std::vector<uint32_t>& indexVec, 
			const std::function<void(const std::string&, uint32_t, uint32_t, uint32_t)>& addSubmesh);

	private:
		std::unique_ptr<VertexBuffer> pVertexBuffer;
		std::unique_ptr<IndexBuffer> pIndexBuffer;
		std::vector<Submesh> submeshVec;
		const Renderer& renderer;

	private:
		static std::unique_ptr<MeshHandler> ptrMeshHandler;
	};
}
