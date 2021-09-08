#ifndef RDR_RENDEROBJECT_H
#define RDR_RENDEROBJECT_H
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>
#include <d3d12.h>
using Microsoft::WRL::ComPtr;

namespace rdr
{
	struct RenderObject
	{
		uint32_t psoIndex;
		uint32_t passIndex;
		uint32_t meshIndex;
		uint32_t renderTargetCount;
		uint32_t depthStencilIndex;
		bool needClearRenderTarget;
		bool needClearDepthBuffer;
	};
}

#endif
