#pragma once
#include "IConstantBuffer.h"
#include <memory>
#include <vector>
#include "ConstantBuffer.h"
#include <utility>
namespace rdr 
{
	class Camera;

	class ConstBufferPool
	{
	public:
		ConstBufferPool(const RendererFacade& renderer);
		~ConstBufferPool();
		void Update(const Camera& camera);

		const ConstantBuffer<ObjectConstant>& GetObjCBuffer(uint32_t index) const
		{
			return objConstBufferVec[index];
		}

		const ConstantBuffer<PassConstant>& GetPassCBuffer(uint32_t index) const
		{
			return passConstBufferVec[index];
		}
		const ConstantBuffer<MaterialConstant>& GetMatCBuffer(uint32_t index) const
		{
			return matConstBufferVec[index];
		}

		uint32_t AddMaterial(ConstantBuffer<MaterialConstant>& mat);
		uint32_t AddMaterial(const std::string_view& meshName, const RendererFacade& renderer);

	private:
		std::vector<ConstantBuffer<ObjectConstant>> objConstBufferVec;
		std::vector<ConstantBuffer<PassConstant>> passConstBufferVec;
		std::vector<ConstantBuffer<MaterialConstant>> matConstBufferVec;
	};
}