#pragma once
#include "ConstValue.h"
#include "IResource.h"
#include <vector>
namespace rdr
{
	class Mesh;
	class HeapResourceManager;

	class IndexBuffer : public IResource
	{
		friend HeapResourceManager;
		friend Mesh;

	private:
		std::vector<uint32_t> indexVec;
		ComPtr<ID3D12Resource> pIntermediateResource;
	};
}