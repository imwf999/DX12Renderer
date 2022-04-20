#include "GBuffer.h"
#include "Commond.h"
#include "DescriptorPool.h"
#include "Display.h"
#include "Material.h"
#include "MaterialPool.h"
#include "Renderer.h"
#include "RenderTexture.h"
#include "RootSignature.h"
#include "Shader.h"
#include "TexturePool.h"
#include "Mesh.h"
#include "PSO.h"

namespace rdr
{
	GBuffer::GBuffer(const Renderer& renderer)
		: ptrScreenSpaceNormal(std::dynamic_pointer_cast<RenderTexture>(renderer.GetTexPool()->GetTexture("ScreenNormal"))),
	ptrDepthTexture(std::dynamic_pointer_cast<DepthTexture>(renderer.GetTexPool()->GetTexture("SsaoDepthTex"))),
	ptrMaterial(renderer.GetMaterialPool()->GetMaterial("DrawNormal")),
	ptrRenderer(&renderer)
	{

	}

	GBuffer::~GBuffer()
	{

	}

	void GBuffer::Draw()
	{
		ID3D12GraphicsCommandList* pList = ptrRenderer->GetCommond()->GetCmdList();
		Display* pDisplay = ptrRenderer->GetDisplay();
		DescriptorPool* pDescriptorPool = ptrRenderer->GetDescPool();
		const auto& mesh = ptrRenderer->GetMeshVec();
		pList->RSSetViewports(1, &pDisplay->GetViewPort());
		pList->RSSetScissorRects(1, &pDisplay->GetRect());
		auto& RTV = pDescriptorPool->GetRtvStart().Offset(ptrScreenSpaceNormal->GetRtvIndex(), pDescriptorPool->GetRtvDescSize());
		auto& DSV = pDescriptorPool->GetDsvStart().Offset(ptrDepthTexture->GetDsvIndex(), pDescriptorPool->GetDsvDescSize());
		pList->OMSetRenderTargets(1, &RTV.CpuHandle(), false, &DSV.CpuHandle());
		pList->ClearRenderTargetView(RTV.CpuHandle(), DirectX::Colors::LightSteelBlue, 0, nullptr);
		pList->ClearDepthStencilView(DSV.CpuHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		const auto& shader = ptrMaterial->GetShader();
		pList->SetGraphicsRootSignature(shader->GetRootSignature().GetSignature());
		pList->SetPipelineState(shader->GetPSO().GetPSO());
		uint32_t length = mesh.size() - 1;
		for (uint32_t index = 0; index < length; ++index)
		{
			mesh[index]->DrawByIndex(pList, pDescriptorPool, *ptrRenderer->GetFrameCBuffer(), *ptrMaterial);
		}
		ptrDepthTexture->SetStateFromDepthWriteToGenericRead(pList);
		ptrScreenSpaceNormal->SetStateFromRenderTargetToShaderResource(pList);
	}
}
