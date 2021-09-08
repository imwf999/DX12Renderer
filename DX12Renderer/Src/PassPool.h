#pragma once
#include "Pass.h"
namespace rdr
{
	class PassPool
	{
	public:
		PassPool(ID3D12Device* pDevice);
		~PassPool();

		Pass& operator[](size_t index)
		{
			return passVec[index];
		}

		const Pass& operator[](size_t index) const
		{
			return passVec[index];
		}

	private:
		std::vector<Pass> passVec;
	};
}