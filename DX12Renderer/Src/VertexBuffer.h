#pragma once
#include "IResource.h"
#include <DirectXMath.h>
#include <vector>
namespace rdr
{
	using namespace DirectX;
	class Mesh;
	class HeapResourceManager;

	struct VertexData
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
	};

	class VertexBuffer final : public IResource
	{
		friend HeapResourceManager;
		friend Mesh;

	public:
		VertexBuffer() = default;
		~VertexBuffer() = default;

	private:
		ComPtr<ID3D12Resource> pIntermediateResource;
		std::vector<VertexData> vertexData;
	};
}
