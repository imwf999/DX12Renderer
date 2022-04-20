#include "Display.h"
#include "RendererHelper.h"
#include "Game.h"
#include "RenderTexture.h"
#include "Renderer.h"
#include "Commond.h"
#include "DescriptorPool.h"
namespace rdr
{
	Display::Display(IDXGIFactory5* pDxgi, ID3D12Device4* pDev, IDXGIAdapter1* pAda, const Renderer& renderer)
		: pIDXGIFactory5(pDxgi), pd3dDevice(pDev), pAdapter(pAda)
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		swapChainDesc.BufferCount = swapChainBufferCount;				//交换链中包含的后台缓冲区个数
		swapChainDesc.Width = global_WindowWidth;
		swapChainDesc.Height = global_WindowHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		ThrowIfFailed(pIDXGIFactory5->CreateSwapChainForHwnd(
			renderer.GetCommond()->GetCmdQueue(),
			Game::Instance()->GetMainWinHandle(),
			&swapChainDesc,
			nullptr,
			nullptr,
			pSwapChain1.GetAddressOf()
		));

		ThrowIfFailed(pSwapChain1.As(&pSwapChain3));
		currentBackBufferIndex = pSwapChain3->GetCurrentBackBufferIndex();		//获取当前的后台缓冲区序号，SwapChain3中扩展的方法

		//创建RTV描述符
		for (int i = 0; i < swapChainBufferCount; ++i)
		{
			pSwapChainBufferArr[i] = std::make_unique<RenderTexture>("BackBuffer" + std::to_string(i));
			ThrowIfFailed(pSwapChain3->GetBuffer(i, IID_PPV_ARGS(pSwapChainBufferArr[i]->GetDefaultResourceAddressOf())));
			renderer.GetDescPool()->CreateRTV(this->pSwapChainBufferArr[i]->GetDefaultResource(), nullptr);
		}
	}

	Display::~Display() = default;

	void Display::Present(const Renderer& renderer)
	{
		auto* pList = renderer.GetCommond()->GetCmdList();
		auto* pQueue = renderer.GetCommond()->GetCmdQueue();

		pSwapChainBufferArr[currentBackBufferIndex]->SetStateFromRenderTargetToPresent(pList);
		ThrowIfFailed(pList->Close());

		ID3D12CommandList* cmdList2[] = { pList };
		pQueue->ExecuteCommandLists(_countof(cmdList2), cmdList2);
		ThrowIfFailed(pSwapChain3->Present(0, 0));

		renderer.GetCommond()->WaitForGPU();
		currentBackBufferIndex = pSwapChain3->GetCurrentBackBufferIndex();
	}

	void Display::SetSwapChainStateToRenderTarget(const Renderer& renderer)
	{
		pSwapChainBufferArr[currentBackBufferIndex]->SetStateFromPresentToRenderTarget(renderer.GetCommond()->GetCmdList());
	}
}