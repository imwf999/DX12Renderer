#pragma once
#include "RendererHelper.h"
#include "Renderer.h"
#include "HeapManager.h"
#include "BaseResource.h"

namespace rdr
{
	class ConstantBuffer final : public BaseResource
	{
	public:
		ConstantBuffer(const std::string& InName) : pConstValue(nullptr), constBufferSize(0), name(InName) {  }
		ConstantBuffer(const ConstantBuffer& other) = delete;
		ConstantBuffer(ConstantBuffer&& buffer) noexcept;
		~ConstantBuffer() override;

		template<typename Ty_Data>
		void CreateConstBuffer(const Renderer& renderer, const Ty_Data& value = Ty_Data())
		{
			constBufferSize = UpperAlignment(sizeof(Ty_Data), 256);
			SetResourceDescAsBuffer(constBufferSize);
			renderer.GetResMgr()->CreateBufferInUploadHeap(this, constBufferSize);
			pConstValue = nullptr;
			ThrowIfFailed(pResource->Map(0, nullptr, &pConstValue));
			memcpy(pConstValue, &value, constBufferSize);
		}

		void CreateConstBuffer(const Renderer& renderer, uint32_t size, const void* ptrData = nullptr);

		template<typename Ty_Data>
		void Update(const Ty_Data& constValue)
		{
			memcpy(pConstValue, &constValue, constBufferSize);
		}

		void Update(uint32_t offset, const void* ptrSrc, uint32_t copySize)
		{
			char* tempPtr = static_cast<char*>(pConstValue);
			tempPtr += offset;
			memcpy(tempPtr, ptrSrc, copySize);
		}

		const void* GetConstPtr() const { return pConstValue; }

	private:
		std::string name;
		void* pConstValue;
		uint32_t constBufferSize;
	};
}
