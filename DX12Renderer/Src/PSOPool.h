#ifndef RDR_PSOPOOL_H
#define RDR_PSOPOOL_H
#include <memory>
#include <wrl.h>
#include <string>
#include <vector>
#include <d3d12.h>

namespace rdr
{
	class IPipelineState;
	class RendererFacade;

	class PSOPool
	{
	public:
		PSOPool(const RendererFacade& renderer);
		~PSOPool();

		IPipelineState* operator[](size_t index) const
		{
			return psoVec[index].get();
		}
	private:
		std::vector<std::unique_ptr<IPipelineState>> psoVec;
	};
}

#endif
