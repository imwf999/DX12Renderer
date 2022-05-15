#include "Commond.h"
#include "RendererHelper.h"

namespace rdr
{
	Commond::Commond(ID3D12Device4* pd3dDevice)
		: fenceValue(0)
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		CHECK_RESULT(pd3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pCommandQueue)));

		CHECK_RESULT(pd3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(pCommandAlloc.GetAddressOf())
		));

		CHECK_RESULT(pd3dDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			pCommandAlloc.Get(),
			nullptr,
			IID_PPV_ARGS(pCommandList.GetAddressOf())
		));

		CHECK_RESULT(pd3dDevice->CreateFence(
			0,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(pFence.GetAddressOf())
		));

		fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (fenceEvent == nullptr)
			CHECK_RESULT(HRESULT_FROM_WIN32(GetLastError()));
	}

	Commond::~Commond()
	{
	}
}