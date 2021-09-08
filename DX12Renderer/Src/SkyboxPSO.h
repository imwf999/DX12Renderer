#pragma once
#include "IPipelineState.h"
namespace rdr {

class SkyboxPSO final : public IPipelineState
{
public:
	SkyboxPSO()
	{

	}
	~SkyboxPSO() = default;

public:
	void Create(const Pass& pass, ID3D12Device4* device) override;
};
}