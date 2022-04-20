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

namespace rdr
{
	Mesh::Mesh()
		: pVertexBuffer(std::make_unique<VertexBuffer>()),
		pIndexBuffer(std::make_unique<IndexBuffer>()), pModel(nullptr),
		ptrAiImportor(std::make_unique<Assimp::Importer>())
	{ }

	Mesh::Mesh(const Renderer& renderer, uint32_t InVertexNum, const std::string& InMatName,
	           uint32_t InStartIndexLocation, uint32_t InBaseVertexLocation): pModel(nullptr)
	{
		Submesh mesh;
		mesh.material = renderer.GetMaterialPool()->GetMaterial(InMatName);
		mesh.vertexCount = InVertexNum;
		mesh.startIndexLocation = InStartIndexLocation;
		mesh.baseVertexLocation = InBaseVertexLocation;
		submeshVec.emplace_back(mesh);
	}

	Mesh::Mesh(Mesh&& other) noexcept : pModel(other.pModel)
	{
		this->pVertexBuffer = std::move(other.pVertexBuffer);
		this->pIndexBuffer = std::move(other.pIndexBuffer);
		this->submeshVec = std::move(other.submeshVec);
		this->ptrAiImportor = std::move(other.ptrAiImportor);
	}

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

	void Mesh::DrawByIndex(ID3D12GraphicsCommandList* pList, const DescriptorPool* pDescriptorPool, const FrameConstBuffer& frameBuffer)
	{
		size_t length = submeshVec.size();
		pList->IASetVertexBuffers(0, 1, &pVertexBuffer->BufferView());
		pList->IASetIndexBuffer(&pIndexBuffer->BufferView());
		pList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		auto& shader = submeshVec[0].material->GetShader();
		for (size_t i = 0; i < length; ++i)
		{
			shader->SetRootSignature(pList, pDescriptorPool, *submeshVec[i].material, frameBuffer);
			pList->DrawIndexedInstanced(submeshVec[i].indexCount, 1, submeshVec[i].startIndexLocation, submeshVec[i].baseVertexLocation, 0);
		}
	}

	void Mesh::LoadSponza(const std::string& filePath, const Renderer& renderer)
	{
		std::vector<VertexData> vertexVec;
		std::vector<uint32_t> indexVec;
		LoadSponza(filePath, renderer, vertexVec, indexVec);
		pVertexBuffer->CreateBuffer(renderer, vertexVec);
		pIndexBuffer->CreateBuffer(renderer, indexVec);
		ptrAiImportor->FreeScene();
	}

	void Mesh::LoadSkyBox(const std::string& filePath, const Renderer& renderer)
	{
		std::vector<VertexData> vertexVec;
		std::vector<uint32_t> indexVec;
		pModel = ptrAiImportor->ReadFile(filePath, aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);
		uint64_t vertexLength = 0, indexLength = 0;
		for (unsigned int i = 0; i < pModel->mNumMeshes; ++i)
		{
			vertexLength += pModel->mMeshes[i]->mNumVertices;
			indexLength += pModel->mMeshes[i]->mNumFaces * 3;
		}
		vertexVec.resize(vertexLength);
		indexVec.resize(indexLength);
		const aiMesh* ptrMesh = pModel->mMeshes[0];
		uint32_t vertices = 0, indices = 0;
		for (unsigned int vertexIndex = 0; vertexIndex < ptrMesh->mNumVertices; ++vertexIndex, ++vertices)
		{
			aiVector3D* ptrVertex = ptrMesh->mVertices;
			vertexVec[vertices].position = { ptrVertex[vertexIndex].x, ptrVertex[vertexIndex].y, ptrVertex[vertexIndex].z };
		}
		for (unsigned int faceIndex = 0; faceIndex < ptrMesh->mNumFaces; ++faceIndex)
		{
			for (unsigned int index = 0; index < ptrMesh->mFaces->mNumIndices; ++index, ++indices)
				indexVec[indices] = ptrMesh->mFaces[faceIndex].mIndices[index];
		}
		Submesh submesh = {  };
		submesh.material = renderer.GetMaterialPool()->GetMaterial("SkyBox");
		submesh.indexCount = ptrMesh->mNumFaces * ptrMesh->mFaces->mNumIndices;
		submesh.startIndexLocation = submesh.baseVertexLocation = 0;
		submeshVec.push_back(submesh);
		pVertexBuffer->CreateBuffer(renderer, vertexVec);
		pIndexBuffer->CreateBuffer(renderer, indexVec);
		ptrAiImportor->FreeScene();
	}

	void Mesh::LoadSponza(const std::string& filePath, const Renderer& renderer, std::vector<VertexData>& vertexVec, std::vector<uint32_t>& indexVec)
	{
		pModel = ptrAiImportor->ReadFile(filePath, aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_Triangulate);
		uint64_t vertexLength = 0, indexLength = 0;
		for (unsigned int i = 0; i < pModel->mNumMeshes; ++i)
		{
			vertexLength += pModel->mMeshes[i]->mNumVertices;
			indexLength += pModel->mMeshes[i]->mNumFaces * 3;
		}
		vertexVec.resize(vertexLength + 10);
		indexVec.resize(indexLength + 10);
		uint32_t vertices = 0, indices = 0, vertexOffset = 0, indexOffset = 0;
		for (unsigned int i = 0; i < pModel->mNumMeshes; ++i)
		{
			const aiMesh& mesh = *(pModel->mMeshes[i]);
			for (unsigned int vertexIndex = 0; vertexIndex < mesh.mNumVertices; ++vertexIndex, ++vertices)
			{
				aiVector3D* ptrVertex = pModel->mMeshes[i]->mVertices;
				vertexVec[vertices].position = { ptrVertex[vertexIndex].x, ptrVertex[vertexIndex].y, ptrVertex[vertexIndex].z };
				aiVector3D* ptrNormal = pModel->mMeshes[i]->mNormals;
				vertexVec[vertices].normal = { ptrNormal[vertexIndex].x, ptrNormal[vertexIndex].y, ptrNormal[vertexIndex].z };
				aiVector3D* ptrTangent = pModel->mMeshes[i]->mTangents;
				vertexVec[vertices].tangent = { ptrTangent[vertexIndex].x, ptrTangent[vertexIndex].y, ptrTangent[vertexIndex].z };
				aiVector3D* ptrUV = pModel->mMeshes[i]->mTextureCoords[0];
				vertexVec[vertices].uv = { ptrUV[vertexIndex].x, ptrUV[vertexIndex].y };
			}
			for (unsigned int faceIndex = 0; faceIndex < mesh.mNumFaces; ++faceIndex)
			{
				for (unsigned int index = 0; index < mesh.mFaces->mNumIndices; ++index, ++indices)
					indexVec[indices] = mesh.mFaces[faceIndex].mIndices[index];
			}
			AddSponzaSubmesh(mesh, renderer, indexOffset, vertexOffset);
			vertexOffset = vertices;
			indexOffset = indices;
		}
	}

	void Mesh::AddSponzaSubmesh(const aiMesh& mesh, const Renderer& renderer, uint32_t indexOffset, uint32_t vertexOffset)
	{
		Submesh submesh = {};
		submesh.material = renderer.GetMaterialPool()->AddMaterialFromSponzaMesh(std::string(mesh.mName.C_Str()));
		submesh.indexCount = mesh.mNumFaces * mesh.mFaces->mNumIndices;
		if(submesh.indexCount == 1200)
		{
			int i = 0;
			++i;
		}
		submesh.startIndexLocation = indexOffset;
		submesh.baseVertexLocation = vertexOffset;
		submeshVec.push_back(submesh);
	}
}
