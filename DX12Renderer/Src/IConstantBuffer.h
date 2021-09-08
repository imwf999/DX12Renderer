#pragma once
#include "ConstValue.h"
#include <DirectXMath.h>
#include "d3dx12.h"
#include "RendererFacade.h"
#include "HeapResourceManager.h"
#include "RendererHelper.h"
#include "IResource.h"
namespace rdr
{
	class Camera;
	class IConstantBuffer : public IResource
	{
	public:
		IConstantBuffer() = default;
		virtual ~IConstantBuffer()
		{
			if (pResource != nullptr)
				pResource->Unmap(0, nullptr);
		}

	protected:
		uint32_t constBufferSize;
	};
}


