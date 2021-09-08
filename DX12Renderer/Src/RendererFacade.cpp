#include "RendererFacade.h"
#include "DescriptorPool.h"
#include "PassPool.h"
#include "Display.h"
#include "Commond.h"
#include "Mesh.h"
#include "PSOPool.h"
#include "IPipelineState.h"
#include "Camera.h"
#include "RenderObject.h"
#include "TexturePool.h"
#include "ConstBufferPool.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "DescriptorPool.h"
#include <DirectXColors.h>
#include <map>
namespace rdr
{
	RendererFacade::RendererFacade()
	{
		CreateDevice();
		pCommond = std::make_unique<Commond>(pd3dDevice.Get());
		pDescriptorPool = std::make_unique<DescriptorPool>(this->pd3dDevice, DescriptorPool::DescriptorPoolInitializeDesc());
		pDisplay = std::make_unique<Display>(pIDXGIFactory5.Get(), pd3dDevice.Get(), pAdapter.Get(), *this);
		pHeapResMgr = std::make_unique<HeapResourceManager>(pd3dDevice, global_DefaultHeapSize, global_UploadHeapSize);
		pPassPool = std::make_unique<PassPool>(this->pd3dDevice.Get());
		pPSOPool = std::make_unique<PSOPool>(*this);
		renderObjectVec.push_back({ 1, 1, 0, 0, 1, false, true});
#ifdef POINTSHADOW
		renderObjectVec.push_back({ 1, 3, 0, 0, 2, false, true });
		renderObjectVec.push_back({ 1, 4, 0, 0, 3, false, true });
		renderObjectVec.push_back({ 1, 5, 0, 0, 4, false, true });
		renderObjectVec.push_back({ 1, 6, 0, 0, 5, false, true });
		renderObjectVec.push_back({ 1, 7, 0, 0, 6, false, true });
		renderObjectVec.push_back({ 1, 8, 0, 0, 7, false, true });
#endif
		renderObjectVec.push_back({ 0, 0, 0, 1, 0, true, true });
		renderObjectVec.push_back({ 2, 2, 1, 1, 0, false, false});
		pTexturePool = std::make_unique<TexturePool>(*this);
		pConstBufferPool = std::make_unique<ConstBufferPool>(*this);
		std::unique_ptr<Mesh> miao = std::make_unique<Mesh>("Model\\sponza.obj", *this);
		std::unique_ptr<Mesh> skybox = std::make_unique<Mesh>("Model\\sphere.fbx", *this);
		meshVec.push_back(std::move(miao));
		meshVec.push_back(std::move(skybox));
	}

	void RendererFacade::DrawObject(ID3D12GraphicsCommandList* pList, RenderObject& object)
	{
		auto& RTV = pDescriptorPool->GetRtvStart().Offset(pDisplay->GetCurBackBufferIndex(), pDescriptorPool->GetRtvDescSize());
		auto& DSV = pDescriptorPool->GetDsvStart().Offset(object.depthStencilIndex, pDescriptorPool->GetDsvDescSize());
		pList->OMSetRenderTargets(object.renderTargetCount, (object.renderTargetCount ? &RTV.CpuHandle() : nullptr), false, &DSV.CpuHandle());
		if(object.needClearRenderTarget) pList->ClearRenderTargetView(RTV.CpuHandle(), DirectX::Colors::LightSteelBlue, 0, nullptr);			
		if(object.needClearDepthBuffer) pList->ClearDepthStencilView(DSV.CpuHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		pList->SetGraphicsRootSignature((*pPassPool)[object.passIndex].GetRootSignature());
		pList->SetPipelineState((*pPSOPool)[object.psoIndex]->GetPSO());
		meshVec[object.meshIndex]->Draw(*this, object.passIndex);
	}

	void RendererFacade::Draw()
	{
		pCommond->GetCmdList()->RSSetViewports(1, &pDisplay->GetViewPort());
		pCommond->GetCmdList()->RSSetScissorRects(1, &pDisplay->GetRect());
		pDescriptorPool->SetDescriptorHeap(pCommond->GetCmdList());

		//~~~~~~~~~渲染到阴影图~~~~~~~~~~
		DrawObject(pCommond->GetCmdList(), renderObjectVec[0]);
		pDisplay->SetShadowMapResource(pCommond->GetCmdList(), pTexturePool->GetMainShadowMap());
		int i = 0;
#ifdef POINTSHADOW
		for (; i < 6; ++i)
		{
			DrawObject(pCommond->GetCmdList(), renderObjectVec[i + 1]);
		}
		pDisplay->SetShadowMapResource(pCommond->GetCmdList(), pTexturePool->GetPointShadowMap());
#endif
		//~~~~~~~~~~渲染到屏幕~~~~~~~~~~~
		pDisplay->SetRenderTarget(*this);
		DrawObject(pCommond->GetCmdList(), renderObjectVec[i + 1]);
		DrawObject(pCommond->GetCmdList(), renderObjectVec[i + 2]);

		//~~~~~~~~~~~~呈现~~~~~~~~~~~~~
		pDisplay->Present(*this);

		//~~~~~~~~~~~~复用命令分配器~~~~~~
		pCommond->Reset((*pPSOPool)[renderObjectVec[2].psoIndex]->GetPSO());
	}

	void RendererFacade::Update(const Camera& camera)
	{
		pConstBufferPool->Update(camera);
	}

	void RendererFacade::CreateDevice()
	{
		//遍历所有显卡，找显存最大的使用, 通常那个是主显卡
		ThrowIfFailed(CreateDXGIFactory(IID_PPV_ARGS(pIDXGIFactory5.GetAddressOf())));
		std::map<size_t, int> adapters;
		for (int i = 0; DXGI_ERROR_NOT_FOUND != pIDXGIFactory5->EnumAdapters1(i, pAdapter.GetAddressOf()); ++i)
		{
			DXGI_ADAPTER_DESC1 desc = {  };
			pAdapter->GetDesc1(&desc);
			adapters.insert({ desc.DedicatedVideoMemory, i });
		}
		pIDXGIFactory5->EnumAdapters1(adapters.rbegin()->second, pAdapter.GetAddressOf());
		ThrowIfFailed(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&pd3dDevice)));
	}

	RendererFacade::~RendererFacade()
	{

	}
}