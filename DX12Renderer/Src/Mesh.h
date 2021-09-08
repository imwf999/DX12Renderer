#ifndef RDR_IMESH_H
#define RDR_IMESH_H
#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <string>
#include <memory>
using Microsoft::WRL::ComPtr;
struct aiScene;

namespace rdr
{
	class RendererFacade;
	class VertexBuffer;
	class IndexBuffer;

	class Mesh
	{
	public:
		Mesh(const std::string& filePath, const RendererFacade& renderer);
		~Mesh() = default;

	private:
		struct Submesh
		{
			uint32_t materialIndex;
			uint32_t indexCount;
			uint32_t startIndexLocation;
			uint32_t baseVertexLocation;
		};

	public:
		void Draw(const RendererFacade& renderer, uint32_t passIndex);
		const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView; }
		const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return indexBufferView; }
		const std::vector<Submesh>& GetSubmeshData() const { return submeshVec; }

	public:
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		std::unique_ptr<VertexBuffer> pVertexBuffer;
		std::unique_ptr<IndexBuffer> pIndexBuffer;
		std::vector<Submesh> submeshVec;
		uint32_t objectCBufferIndex;
	};
}

#endif
