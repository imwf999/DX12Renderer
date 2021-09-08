#include "Mesh.h"
#include "PassPool.h"
#include "Commond.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstBufferPool.h"
#include "RendererFacade.h"
#include "HeapResourceManager.h"
#include "ConstantBuffer.h"
#include "TexturePool.h"
#include "Texture.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
namespace rdr
{
	Mesh::Mesh(const std::string& filePath, const RendererFacade& renderer)
	{
		Assimp::Importer aiImportor;
		const aiScene* pModel = aiImportor.ReadFile(filePath, aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs );
		pVertexBuffer = std::make_unique<VertexBuffer>();
		pIndexBuffer = std::make_unique<IndexBuffer>();
		uint64_t vertexLength = 0, indexLength = 0;
		for (unsigned int i = 0; i < pModel->mNumMeshes; ++i)
		{
			vertexLength += pModel->mMeshes[i]->mNumVertices;
			indexLength += pModel->mMeshes[i]->mNumFaces * 3;
		}
		pVertexBuffer->vertexData.resize(vertexLength);
		pIndexBuffer->indexVec.resize(indexLength);
		uint32_t vertices = 0, indices = 0, vertexOffset = 0, indexOffset = 0;
		std::vector<VertexData>& vertexVec = pVertexBuffer->vertexData;
		for (unsigned int i = 0; i < pModel->mNumMeshes; ++i)
		{
			aiMesh& mesh = *(pModel->mMeshes[i]);
			for (unsigned int vertexIndex = 0; vertexIndex < mesh.mNumVertices; ++vertexIndex)
			{
				vertexVec[vertices].position.x = pModel->mMeshes[i]->mVertices[vertexIndex].x;
				vertexVec[vertices].position.y = pModel->mMeshes[i]->mVertices[vertexIndex].y;
				vertexVec[vertices].position.z = pModel->mMeshes[i]->mVertices[vertexIndex].z;
				vertexVec[vertices].normal.x = pModel->mMeshes[i]->mNormals[vertexIndex].x;
				vertexVec[vertices].normal.y = pModel->mMeshes[i]->mNormals[vertexIndex].y;
				vertexVec[vertices].normal.z = pModel->mMeshes[i]->mNormals[vertexIndex].z;
				vertexVec[vertices].tangent.x = pModel->mMeshes[i]->mTangents[vertexIndex].x;
				vertexVec[vertices].tangent.y = pModel->mMeshes[i]->mTangents[vertexIndex].y;
				vertexVec[vertices].tangent.z = pModel->mMeshes[i]->mTangents[vertexIndex].z;
				vertexVec[vertices].uv.x = pModel->mMeshes[i]->mTextureCoords[0][vertexIndex].x;
				vertexVec[vertices].uv.y = pModel->mMeshes[i]->mTextureCoords[0][vertexIndex].y;
				++vertices;
			}
			for (unsigned int faceIndex = 0; faceIndex < mesh.mNumFaces; ++faceIndex)
			{
				for (unsigned int index = 0; index < mesh.mFaces->mNumIndices; ++index, ++indices)
					pIndexBuffer->indexVec[indices] = mesh.mFaces[faceIndex].mIndices[index];
			}
			Submesh submesh = {};
			submesh.materialIndex = renderer.GetCBufferPool()->AddMaterial(std::string_view(mesh.mName.C_Str()), renderer);
			submesh.indexCount = mesh.mNumFaces * mesh.mFaces->mNumIndices;
			submesh.startIndexLocation = indexOffset;
			submesh.baseVertexLocation = vertexOffset;
			submeshVec.push_back(submesh);

			vertexOffset = vertices;
			indexOffset = indices;
		}
		uint32_t vertexBufferSize = static_cast<uint32_t>(pVertexBuffer->vertexData.size() * sizeof(VertexData));
		renderer.GetResMgr()->CreateBufferResourceInDefaultHeap(
			reinterpret_cast<void*>(pVertexBuffer->vertexData.data()),
			vertexBufferSize,
			nullptr,
			pVertexBuffer->pResource,
			pVertexBuffer->pIntermediateResource,
			renderer.GetCommond()->GetCmdList());
		vertexBufferView.BufferLocation = pVertexBuffer->GetVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(VertexData);
		vertexBufferView.SizeInBytes = vertexBufferSize;
		uint32_t indexBufferSize = static_cast<uint32_t>(pIndexBuffer->indexVec.size() * sizeof(uint32_t));
		renderer.GetResMgr()->CreateBufferResourceInDefaultHeap(
			reinterpret_cast<void*>(pIndexBuffer->indexVec.data()),
			indexBufferSize,
			nullptr,
			pIndexBuffer->pResource,
			pIndexBuffer->pIntermediateResource,
			renderer.GetCommond()->GetCmdList());
		indexBufferView.BufferLocation = pIndexBuffer->GetVirtualAddress();
		indexBufferView.SizeInBytes = indexBufferSize;
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}

	void Mesh::Draw(const RendererFacade& renderer, uint32_t passIndex)
	{
		size_t length = submeshVec.size();
		for (size_t i = 0; i < length; ++i)
		{
			ID3D12GraphicsCommandList* list = renderer.GetCommond()->GetCmdList();
			(*renderer.GetPassPool())[passIndex].SetRootParameter(list, renderer.GetDescPool(), renderer.GetCBufferPool(), submeshVec[i].materialIndex);
			list->IASetVertexBuffers(0, 1, &vertexBufferView);
			list->IASetIndexBuffer(&indexBufferView);
			list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			list->DrawIndexedInstanced(submeshVec[i].indexCount, 1, submeshVec[i].startIndexLocation, submeshVec[i].baseVertexLocation, 0);
		}
	}
}
