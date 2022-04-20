#include "VertexBuffer.h"
#include "Commond.h"
#include "HeapManager.h"
#include "Renderer.h"

namespace rdr
{
	VertexBuffer::~VertexBuffer()
	{
	}

	void VertexBuffer::CreateBuffer(const Renderer& renderer, const std::vector<VertexData>& vertexData)
	{
		uint32_t vertexBufferSize = static_cast<uint32_t>(vertexData.size() * sizeof(VertexData));
		SetResourceDescAsBuffer(vertexBufferSize);
		renderer.GetResMgr()->CreateBufferInDefaultHeap(vertexData.data(),this, renderer.GetCommond()->GetCmdList());
		bufferView.BufferLocation = GetVirtualAddress();
		bufferView.StrideInBytes = sizeof(VertexData);
		bufferView.SizeInBytes = vertexBufferSize;
	}
}
