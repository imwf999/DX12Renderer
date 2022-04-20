#pragma once
#include <DirectXMath.h>
#include <vector>
#include "BaseResource.h"

namespace rdr
{
	using namespace DirectX;
	class Mesh;
	class Renderer;
	class HeapManager;

	struct VertexData
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
	};

	class VertexBuffer final : public BaseResource
	{
	public:
		VertexBuffer() = default;
		~VertexBuffer() override;

	public:
		void CreateBuffer(const Renderer& renderer, const std::vector<VertexData>& vertexData);
		const D3D12_VERTEX_BUFFER_VIEW& BufferView() const { return bufferView; }

	private:
		D3D12_VERTEX_BUFFER_VIEW bufferView;
	};
}
