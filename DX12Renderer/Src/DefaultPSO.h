#pragma once
#include "IPipelineState.h"
namespace rdr {
class Pass;

class DefaultPSO final : public IPipelineState
{
public:
	DefaultPSO()
	{

	}
	virtual ~DefaultPSO() = default;

public:
	void Create(const Pass& pass, ID3D12Device4* device) override;
};
}
