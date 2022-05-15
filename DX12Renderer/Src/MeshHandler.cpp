#include "MeshHandler.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fstream>
#include "RendererHelper.h"
#include "VertexBuffer.h"

namespace rdr
{
	MeshHandler::MeshHandler()
	{

	}

	MeshHandler::~MeshHandler()
	{

	}

	void MeshHandler::ConvertMesh(const std::string& filePath, const std::string& newFilePath)
	{
		std::ofstream fileOut;
		fileOut.open(newFilePath, std::ios::out | std::ios::binary);
		if (!fileOut) DX_THROW("Open File Error");

		Assimp::Importer importer;
		const aiScene* pModel = importer.ReadFile(filePath, aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_Triangulate);

		fileOut.write(reinterpret_cast<const char*>(&pModel->mNumMeshes), sizeof(pModel->mNumMeshes));

		uint64_t vertexLength = 0, indexLength = 0;
		for (unsigned int i = 0; i < pModel->mNumMeshes; ++i)
		{
			vertexLength += pModel->mMeshes[i]->mNumVertices;
			indexLength += pModel->mMeshes[i]->mNumFaces * 3;
		}

		fileOut.write(reinterpret_cast<char*>(&vertexLength), sizeof(vertexLength));
		fileOut.write(reinterpret_cast<char*>(&indexLength), sizeof(indexLength));

		uint32_t vertices = 0, indices = 0, vertexOffset = 0, indexOffset = 0;
		for (unsigned int i = 0; i < pModel->mNumMeshes; ++i)
		{
			const aiMesh& mesh = *(pModel->mMeshes[i]);
			uint32_t meshNameLen = mesh.mName.length;

			fileOut.write(reinterpret_cast<char*>(&meshNameLen), sizeof(meshNameLen));
			fileOut.write(mesh.mName.C_Str(), sizeof(char) * mesh.mName.length);

			fileOut.write(reinterpret_cast<char*>(&vertexOffset), sizeof(vertexOffset));
			fileOut.write(reinterpret_cast<char*>(&indexOffset), sizeof(indexOffset));

			fileOut.write(reinterpret_cast<const char*>(&mesh.mNumVertices), sizeof(mesh.mNumVertices));
			std::vector<VertexData> tVertexVec(mesh.mNumVertices);
			for (unsigned int vertexIndex = 0; vertexIndex < mesh.mNumVertices; ++vertexIndex, ++vertices)
			{
				aiVector3D vertex = mesh.mVertices[vertexIndex], normal = mesh.mNormals[vertexIndex], tangent = mesh.mTangents[vertexIndex], uv = mesh.mTextureCoords[0][vertexIndex];
				tVertexVec[vertexIndex] = { vertex.x, vertex.y, vertex.z, uv.x, uv.y, normal.x, normal.y, normal.z, tangent.x, tangent.y, tangent.z };
			}
			fileOut.write(reinterpret_cast<char*>(tVertexVec.data()), sizeof(VertexData) * mesh.mNumVertices);

			uint64_t tIndexNum = static_cast<uint64_t>(mesh.mNumFaces) * static_cast<uint64_t>(mesh.mFaces->mNumIndices);
			fileOut.write(reinterpret_cast<const char*>(&tIndexNum), sizeof(tIndexNum));
			std::vector<uint32_t> indexVec(tIndexNum);
			uint64_t tIndex = 0;
			for (unsigned int faceIndex = 0; faceIndex < mesh.mNumFaces; ++faceIndex)
			{
				for (unsigned int index = 0; index < mesh.mFaces->mNumIndices; ++index, ++indices, ++tIndex)
					indexVec[tIndex] = mesh.mFaces[faceIndex].mIndices[index];
			}
			fileOut.write(reinterpret_cast<char*>(indexVec.data()), sizeof(uint32_t) * tIndexNum);

			vertexOffset = vertices;
			indexOffset = indices;
		}

		fileOut.close();

		importer.FreeScene();
	}
}
