#pragma once
#include "IPipelineState.h"
namespace rdr {
class ShadowMapPSO final: public IPipelineState
{
public:
	ShadowMapPSO() 
	{

	}
	~ShadowMapPSO() = default;
public:
	void Create(const Pass& pass, ID3D12Device4* device) override;
};
}

