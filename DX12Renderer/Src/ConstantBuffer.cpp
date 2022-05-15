#include "ConstantBuffer.h"

namespace rdr
{
	ConstantBuffer::ConstantBuffer(ConstantBuffer&& buffer) noexcept
		: pConstValue(buffer.pConstValue), constBufferSize(buffer.constBufferSize),
		name(std::move(buffer.name))
	{
		this->pResource = std::move(buffer.pResource);
		this->resourceDesc = buffer.resourceDesc;
		buffer.pResource = nullptr;
		buffer.pConstValue = nullptr;
	}

	ConstantBuffer::~ConstantBuffer()
	{
		if (pResource != nullptr)
			pResource->Unmap(0, nullptr);
	}

	void ConstantBuffer::CreateConstBuffer(const Renderer& renderer, uint32_t size, const void* ptrData)
	{
		constBufferSize = UpperAlignment(size, 256);
		SetResourceDescAsBuffer(constBufferSize);
		renderer.GetResMgr()->CreateBufferInUploadHeap(this, constBufferSize);
		pConstValue = nullptr;
		CHECK_RESULT(pResource->Map(0, nullptr, &pConstValue));
		if (ptrData != nullptr) memcpy(pConstValue, ptrData, size);
	}

}
