#include "Renderer.h"
#include "PSO.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "DescriptorPool.h"
#include "Display.h"
#include "Commond.h"
#include "Mesh.h"
#include "Camera.h"
#include "TexturePool.h"
#include <DirectXColors.h>
#include <map>
#include "Audio.h"
#include "FrameConstBuffer.h"
#include "GBuffer.h"
#include "HeapManager.h"
#include "Material.h"
#include "RootSignature.h"
#include "Shader.h"
#include "ShaderPool.h"
#include "Shadow.h"
#include "SSAO.h"
#include "MaterialPool.h"
#include "UIRenderer.h"
#include "Game.h"
#include "UIManager.h"

namespace rdr
{
	Renderer::Renderer()
	{
		CreateDevice();
		pCommond = std::make_unique<Commond>(pd3dDevice.Get());
		pShaderPool = std::make_unique<ShaderPool>(pd3dDevice.Get());
		pDescriptorPool = std::make_unique<DescriptorPool>(this->pd3dDevice, DescriptorPool::DescriptorPoolInitializeDesc());
		pDisplay = std::make_unique<Display>(pIDXGIFactory5.Get(), pd3dDevice.Get(), pAdapter.Get(), *this);
		pHeapResMgr = std::make_unique<HeapManager>(pd3dDevice, global_DefaultHeapSize, global_UploadHeapSize);
		pTexturePool = std::make_unique<TexturePool>(*this);
		pMaterialPool = std::make_unique<MaterialPool>(*this);
		LoadMesh();
		ptrGBuffer = std::make_unique<GBuffer>(*this);
#if SSAO
		ptrSSAOMap = std::make_unique<SSAOMap>(*this);
#endif
		ptrShadowMap = std::make_unique<Shadow>(*this);
		ptrFrameCBuffer = std::make_unique<FrameConstBuffer>(*this);
		MainDSVIndex = dynamic_cast<DepthTexture*>(pTexturePool->GetTexture("PhongDepthTex").get())->GetDsvIndex();
		ptrSkyBoxPso = &pShaderPool->GetShader("SkyBox")->GetPSO();
		ptrSSRMaterial = pMaterialPool->GetMaterial("SSR");
		ptrUIRenderer = std::make_unique<UIRenderer>(*this);

		//Game::Instance()->GetAudio()->Play(1);
	}


	void Renderer::DrawMesh(ID3D12GraphicsCommandList* pList)
	{
		//TODO:这里获取后台缓冲区的索引直接拿来当rtv索引，前提是后台缓冲区一定比TexturePool先初始化，不然这里会出问题，待修改
		pCommond->GetCmdList()->RSSetViewports(1, &pDisplay->GetViewPort());
		pCommond->GetCmdList()->RSSetScissorRects(1, &pDisplay->GetRect());
		auto& RTV = pDescriptorPool->GetRtvStart().Offset(pDisplay->GetCurrentBackBufferIndex(), pDescriptorPool->GetRtvDescSize());
		auto& DSV = pDescriptorPool->GetDsvStart().Offset(MainDSVIndex, pDescriptorPool->GetDsvDescSize());
		pList->OMSetRenderTargets(1, &RTV.CpuHandle(), false, &DSV.CpuHandle());
		pList->ClearRenderTargetView(RTV.CpuHandle(), DirectX::Colors::LightSteelBlue, 0, nullptr);
		pList->ClearDepthStencilView(DSV.CpuHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		//Draw Sponza
		auto& shader = ptrSSRMaterial->GetShader();
		pList->SetGraphicsRootSignature(shader->GetRootSignature().GetSignature());
		pList->SetPipelineState(shader->GetPSO().GetPSO());
		meshVec[0]->DrawByIndex(pList, pDescriptorPool.get(), *ptrFrameCBuffer, *ptrSSRMaterial);

		//Draw SkyBox
		auto& skyBoxShader = meshVec[1]->GetSubmeshData()[0].materialVec[0]->GetShader();
		pList->SetGraphicsRootSignature(skyBoxShader->GetRootSignature().GetSignature());
		pList->SetPipelineState(skyBoxShader->GetPSO().GetPSO());
		meshVec[1]->DrawByIndex(pList, pDescriptorPool.get(), *ptrFrameCBuffer, 0);

	}

	void Renderer::Draw()
	{
		pDescriptorPool->SetDescriptorHeap(pCommond->GetCmdList());

		//~~~~~~~~~~渲染阴影图~~~~~~~~~~~
		ptrShadowMap->DrawShadowMap();


		//~~~~~~~~~渲染摄像机视角下的深度与法线图~~~~~~~~
		ptrGBuffer->Draw();

#if SSAO
		//~~~~~~~~~渲染并模糊SSAO图~~~~~~~~
		ptrSSAOMap->Draw();
#endif

		pDisplay->SetSwapChainStateToRenderTarget(*this);

		//~~~~~~~~~渲染场景~~~~~~~~~~
		DrawMesh(pCommond->GetCmdList());

		//~~~~~~~~呈现后台缓冲区~~~~~~~
		pDisplay->Present(*this);

		//~~~~~~~~~~复用命令分配器~~~~~~
		pCommond->Reset(ptrSkyBoxPso->GetPSO());
	}

	void Renderer::Update(const Camera& camera)
	{
		ptrFrameCBuffer->Update();
		pMaterialPool->UpdateMaterialData();
	}

	void Renderer::CreateDevice()
	{
		//遍历所有显卡，找显存最大的使用, 通常那个是主显卡
		CHECK_RESULT(CreateDXGIFactory(IID_PPV_ARGS(pIDXGIFactory5.GetAddressOf())));
		std::map<size_t, int> adapters;
		for (int i = 0; DXGI_ERROR_NOT_FOUND != pIDXGIFactory5->EnumAdapters1(i, pAdapter.GetAddressOf()); ++i)
		{
			DXGI_ADAPTER_DESC1 desc = {  };
			pAdapter->GetDesc1(&desc);
			adapters.insert({ desc.DedicatedVideoMemory, i });
		}
		pIDXGIFactory5->EnumAdapters1(adapters.rbegin()->second, pAdapter.GetAddressOf());
		CHECK_RESULT(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&pd3dDevice)));
	}

	void Renderer::LoadMesh()
	{
		std::shared_ptr<Mesh> sponza = std::make_shared<Mesh>(*this);
		sponza->LoadSponza("Model\\sponza.wasset");
		std::shared_ptr<Mesh> skybox = std::make_shared<Mesh>(*this);
		skybox->LoadSkyBox("Model\\skybox.wasset");
		meshVec.emplace_back(sponza);
		meshVec.emplace_back(skybox);
	}

	Renderer::~Renderer()
	{

	}
}
