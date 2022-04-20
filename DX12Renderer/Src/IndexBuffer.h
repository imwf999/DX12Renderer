#pragma once
#include "ConstValue.h"
#include <vector>
#include "BaseResource.h"

namespace rdr
{
	class Mesh;
	class Renderer;
	class HeapManager;

	class IndexBuffer : public BaseResource
	{
	public:
		IndexBuffer();
		~IndexBuffer() override;

	public:
		void CreateBuffer(const Renderer& renderer, const std::vector<uint32_t>& indexVec);
		const D3D12_INDEX_BUFFER_VIEW& BufferView() const { return indexBufferView; }

	private:
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
	};
}
