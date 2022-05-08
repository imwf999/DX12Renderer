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
	ptrScreenSpaceDiffuse(std::dynamic_pointer_cast<RenderTexture>(renderer.GetTexPool()->GetTexture("GBufferDiffuse"))),
	ptrWorldNormal(std::dynamic_pointer_cast<RenderTexture>(renderer.GetTexPool()->GetTexture("ScreenWorldNormal"))),
	ptrDirectLight(std::dynamic_pointer_cast<RenderTexture>(renderer.GetTexPool()->GetTexture("GBufferDirectLight"))),
	ptrDepthTexture(std::dynamic_pointer_cast<DepthTexture>(renderer.GetTexPool()->GetTexture("SsaoDepthTex"))),
	ptrMesh(renderer.GetMeshVec()[0]),
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
		auto& NormalRTV = pDescriptorPool->GetRtvStart().Offset(ptrScreenSpaceNormal->GetRtvIndex(), pDescriptorPool->GetRtvDescSize());
		auto& DiffuseRTV = pDescriptorPool->GetRtvStart().Offset(ptrScreenSpaceDiffuse->GetRtvIndex(), pDescriptorPool->GetRtvDescSize());
		auto& WorldRTV = pDescriptorPool->GetRtvStart().Offset(ptrWorldNormal->GetRtvIndex(), pDescriptorPool->GetRtvDescSize());
		auto& DirectLightRTV = pDescriptorPool->GetRtvStart().Offset(ptrDirectLight->GetRtvIndex(), pDescriptorPool->GetRtvDescSize());
		D3D12_CPU_DESCRIPTOR_HANDLE RTVArr[4] = { NormalRTV.CpuHandle(), DiffuseRTV.CpuHandle(), WorldRTV.CpuHandle(), DirectLightRTV.CpuHandle() };
		auto& DSV = pDescriptorPool->GetDsvStart().Offset(ptrDepthTexture->GetDsvIndex(), pDescriptorPool->GetDsvDescSize());
		pList->OMSetRenderTargets(4, RTVArr, true, &DSV.CpuHandle());
		pList->ClearDepthStencilView(DSV.CpuHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		const auto& shader = ptrMesh->GetSubmeshData()[0].materialVec[0]->GetShader();
		pList->SetGraphicsRootSignature(shader->GetRootSignature().GetSignature());
		pList->SetPipelineState(shader->GetPSO().GetPSO());
		size_t length = mesh.size() - 1;
		for (size_t index = 0; index < length; ++index)
		{
			mesh[index]->DrawByIndex(pList, pDescriptorPool, *ptrRenderer->GetFrameCBuffer(), 0);
		}
		//TODO:多个资源屏障可以优化
		ptrDepthTexture->SetStateFromDepthWriteToGenericRead(pList);
		ptrScreenSpaceNormal->SetStateFromRenderTargetToShaderResource(pList);
		ptrScreenSpaceDiffuse->SetStateFromRenderTargetToShaderResource(pList);
		ptrDirectLight->SetStateFromRenderTargetToShaderResource(pList);
	}
}
