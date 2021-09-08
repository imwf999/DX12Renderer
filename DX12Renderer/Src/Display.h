#pragma once
#include "ConstValue.h"
#include <dxgi1_6.h>
#include <d3d12.h>
#include <memory>
#include "d3dx12.h"
#include "RenderTexture.h"
namespace rdr
{
	using Microsoft::WRL::ComPtr;
	//class RenderTexture;
	class RendererFacade;

	class Display
	{
	public:
		Display(IDXGIFactory5* pDxgi, ID3D12Device4* pDev, IDXGIAdapter1* pAda, const RendererFacade& renderer);
		~Display();

	public:
		void Present(const RendererFacade& renderer);
		void SetRenderTarget(const RendererFacade& renderer);
		void SetShadowMapTarget(ID3D12GraphicsCommandList* pList, ID3D12Resource* pResource);
		void SetShadowMapResource(ID3D12GraphicsCommandList* pList, ID3D12Resource* pResource);
		const D3D12_VIEWPORT& GetViewPort() const { return viewPort; }
		const D3D12_RECT& GetRect() const { return rect; }
		ID3D12Device4* GetDevice() const { return pd3dDevice; }
		uint32_t GetCurBackBufferIndex() const { return currentBackBufferIndex; }

	private:
		static const int swapChainBufferCount = 2;		//交换链中后台缓冲的数量
		uint32_t currentBackBufferIndex;							//当前后台缓冲区的索引
		ComPtr<IDXGISwapChain1> pSwapChain1;
		ComPtr<IDXGISwapChain3> pSwapChain3;
		IDXGIFactory5* pIDXGIFactory5;
		ID3D12Device4* pd3dDevice;
		IDXGIAdapter1* pAdapter;
		D3D12_VIEWPORT viewPort = { 0, 0, static_cast<float>(global_WindowWidth), static_cast<float>(global_WindowHeight), 0, 1.0f };
		D3D12_RECT rect = { 0, 0,static_cast<LONG>(global_WindowWidth), static_cast<LONG>(global_WindowHeight) };
		std::unique_ptr<RenderTexture> pSwapChainBufferArr[swapChainBufferCount];
	};
}