#include "Mesh.h"
#include "PSO.h"
#include "Commond.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "HeapManager.h"
#include "MaterialPool.h"
#include "Material.h"
#include "Shader.h"
#include "FrameConstBuffer.h"
#include "RootSignature.h"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fstream>
#include "MeshHandler.h"

namespace rdr
{
	Mesh::Mesh(const Renderer& InRendere)
		: pVertexBuffer(std::make_unique<VertexBuffer>()),
		pIndexBuffer(std::make_unique<IndexBuffer>()), 
		renderer(InRendere)
	{ }

	Mesh::Mesh(const Renderer& InRenderer, uint32_t InVertexNum, const std::string& InMatName,
	           uint32_t InStartIndexLocation, uint32_t InBaseVertexLocation): renderer(InRenderer)
	{
		Submesh mesh;
		mesh.materialVec.push_back(renderer.GetMaterialPool()->GetMaterial(InMatName));
		mesh.vertexCount = InVertexNum;
		mesh.startIndexLocation = InStartIndexLocation;
		mesh.baseVertexLocation = InBaseVertexLocation;
		submeshVec.emplace_back(mesh);
	}

	Mesh::Mesh(Mesh&& other) noexcept : renderer(other.renderer)
	{
		this->pVertexBuffer = std::move(other.pVertexBuffer);
		this->pIndexBuffer = std::move(other.pIndexBuffer);
		this->submeshVec = std::move(other.submeshVec);
	}

	Mesh::~Mesh() = default;

	std::unique_ptr<MeshHandler> Mesh::ptrMeshHandler = std::make_unique<MeshHandler>();

	void Mesh::DrawByIndex(ID3D12GraphicsCommandList* pList, const DescriptorPool* pDescriptorPool, const FrameConstBuffer& frameBuffer, const Material& material)
	{
		size_t length = submeshVec.size();
		pList->IASetVertexBuffers(0, 1, &pVertexBuffer->BufferView());
		pList->IASetIndexBuffer(&pIndexBuffer->BufferView());
		pList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		auto& shader = material.GetShader();
		for (size_t i = 0; i < length; ++i)
		{
			shader->SetRootSignature(pList, pDescriptorPool, material, frameBuffer);
			pList->DrawIndexedInstanced(submeshVec[i].indexCount, 1, submeshVec[i].startIndexLocation, submeshVec[i].baseVertexLocation, 0);
		}
	}

	void Mesh::DrawByIndex(ID3D12GraphicsCommandList* pList, const DescriptorPool* pDescriptorPool, const FrameConstBuffer& frameBuffer, uint32_t matIndex)
	{
		size_t length = submeshVec.size();
		pList->IASetVertexBuffers(0, 1, &pVertexBuffer->BufferView());
		pList->IASetIndexBuffer(&pIndexBuffer->BufferView());
		pList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		auto& shader = submeshVec[0].materialVec[matIndex]->GetShader();
		for (size_t i = 0; i < length; ++i)
		{
			shader->SetRootSignature(pList, pDescriptorPool, *submeshVec[i].materialVec[matIndex], frameBuffer);
			pList->DrawIndexedInstanced(submeshVec[i].indexCount, 1, submeshVec[i].startIndexLocation, submeshVec[i].baseVertexLocation, 0);
		}
	}

	void Mesh::LoadSponza(const std::string& filePath)
	{
		std::vector<VertexData> vertexVec;
		std::vector<uint32_t> indexVec;
		LoadMesh(filePath, vertexVec, indexVec, [this](const std::string& submeshName, uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset)->void
			{
				AddSponzaSubmesh(submeshName, indexCount, indexOffset, vertexOffset);
			});
		pVertexBuffer->CreateBuffer(renderer, vertexVec);
		pIndexBuffer->CreateBuffer(renderer, indexVec);
	}

	void Mesh::LoadSkyBox(const std::string& filePath)
	{
		std::vector<VertexData> vertexVec;
		std::vector<uint32_t> indexVec;
		LoadMesh(filePath, vertexVec, indexVec, [this](const std::string& submeshName, uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset)->void
			{
				AddSkyBoxSubmesh(submeshName, indexCount, indexOffset, vertexOffset);
			});
		pVertexBuffer->CreateBuffer(renderer, vertexVec);
		pIndexBuffer->CreateBuffer(renderer, indexVec);
	}

	void Mesh::AddSponzaSubmesh(const aiMesh& mesh, uint32_t indexOffset, uint32_t vertexOffset)
	{
		Submesh submesh = {};
		submesh.submeshName = std::string(mesh.mName.C_Str());
		submesh.materialVec = renderer.GetMaterialPool()->AddMaterialFromSponzaMesh(submesh.submeshName);
		submesh.indexCount = mesh.mNumFaces * mesh.mFaces->mNumIndices;
		submesh.startIndexLocation = indexOffset;
		submesh.baseVertexLocation = vertexOffset;
		submeshVec.push_back(submesh);
	}

	void Mesh::AddSponzaSubmesh(const std::string& submeshName, uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset)
	{
		Submesh submesh = {};
		submesh.submeshName = submeshName;
		submesh.materialVec = renderer.GetMaterialPool()->AddMaterialFromSponzaMesh(submesh.submeshName);
		submesh.indexCount = indexCount;
		submesh.startIndexLocation = indexOffset;
		submesh.baseVertexLocation = vertexOffset;
		submeshVec.push_back(submesh);
	}

	void Mesh::AddSkyBoxSubmesh(const std::string& submeshName, uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset)
	{
		Submesh submesh = {  };
		submesh.submeshName = submeshName;
		submesh.materialVec.push_back(renderer.GetMaterialPool()->GetMaterial("SkyBox"));
		submesh.indexCount = indexCount;
		submesh.startIndexLocation = indexOffset;
		submesh.baseVertexLocation = vertexOffset;
		submeshVec.push_back(submesh);
	}

	void Mesh::LoadMesh(const std::string& filePath, std::vector<VertexData>& vertexVec, std::vector<uint32_t>& indexVec, 
		const std::function<void(const std::string&, uint32_t, uint32_t, uint32_t)>& addSubmesh)
	{
		std::ifstream fin;
		fin.open(filePath, std::ios::in | std::ios::binary);
		if (!fin) DX_THROW("Open File Error");

		unsigned int tMeshNum;
		fin.read(reinterpret_cast<char*>(&tMeshNum), sizeof(tMeshNum));

		uint64_t vertexLength, indexLength;
		fin.read(reinterpret_cast<char*>(&vertexLength), sizeof(vertexLength));
		fin.read(reinterpret_cast<char*>(&indexLength), sizeof(indexLength));
		vertexVec.resize(vertexLength + 10);
		indexVec.resize(indexLength + 10);

		uint32_t vertexOffset = 0, indexOffset = 0;

		for (unsigned int i = 0; i < tMeshNum; ++i)
		{
			//TODO: 统一转化为固定类型再存取 
			uint32_t tMeshNameLen;
			std::string tMeshName;
			fin.read(reinterpret_cast<char*>(&tMeshNameLen), sizeof(tMeshNameLen));
			tMeshName.resize(tMeshNameLen);

			unsigned int tMeshVertexNum;
			uint64_t tMeshIndexNum;
			fin.read(const_cast<char*>(tMeshName.c_str()), sizeof(char) * tMeshNameLen);
			fin.read(reinterpret_cast<char*>(&vertexOffset), sizeof(vertexOffset));
			fin.read(reinterpret_cast<char*>(&indexOffset), sizeof(indexOffset));
			fin.read(reinterpret_cast<char*>(&tMeshVertexNum), sizeof(tMeshVertexNum));
			fin.read(reinterpret_cast<char*>(&vertexVec[vertexOffset]), sizeof(VertexData) * tMeshVertexNum);
			fin.read(reinterpret_cast<char*>(&tMeshIndexNum), sizeof(tMeshIndexNum));
			fin.read(reinterpret_cast<char*>(&indexVec[indexOffset]), sizeof(uint32_t) * tMeshIndexNum);

			addSubmesh(tMeshName, static_cast<uint32_t>(tMeshIndexNum), indexOffset, vertexOffset);
		}

		fin.close();
	}

}
