#include "IndexBuffer.h"

#include "Commond.h"
#include "HeapManager.h"
#include "Renderer.h"

namespace rdr
{
	IndexBuffer::IndexBuffer() = default;

	IndexBuffer::~IndexBuffer() = default;

	void IndexBuffer::CreateBuffer(const Renderer& renderer, const std::vector<uint32_t>& indexVec)
	{
		uint32_t indexBufferSize = static_cast<uint32_t>(indexVec.size() * sizeof(uint32_t));
		SetResourceDescAsBuffer(indexBufferSize);
		renderer.GetResMgr()->CreateBufferInDefaultHeap(indexVec.data(),this, renderer.GetCommond()->GetCmdList());
		indexBufferView.BufferLocation = GetVirtualAddress();
		indexBufferView.SizeInBytes = indexBufferSize;
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}
}

