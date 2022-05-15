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
		VertexData& operator= (float arr[11])
		{
			position = { arr[0], arr[1], arr[2] };
			uv = { arr[3], arr[4] };
			normal = { arr[5], arr[6], arr[7] };
			tangent = { arr[8], arr[9], arr[10] };
			return *this;
		}

		VertexData& operator= (const std::initializer_list<float>& listData)
		{
			float arr[11] = { 0 };
			uint32_t index = 0;
			for(auto& item : listData)
			{
				arr[index] = item;
				++index;
			}
			operator=(arr);
			return *this;
		}
		
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
