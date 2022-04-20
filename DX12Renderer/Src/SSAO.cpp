#include "SSAO.h"
#include "PSO.h"
#include "Commond.h"
#include "DescriptorPool.h"
#include "Material.h"
#include "MaterialPool.h"
#include "Renderer.h"
#include "RenderTexture.h"
#include "RootSignature.h"
#include "Shader.h"
#include "TexturePool.h"

namespace rdr
{
	SSAOMap::SSAOMap(const Renderer& renderer)
		: ptrSSAOMap(std::dynamic_pointer_cast<RenderTexture>(renderer.GetTexPool()->GetTexture("SsaoTex"))),
		ptrSSAOBlurMap(std::dynamic_pointer_cast<RenderTexture>(renderer.GetTexPool()->GetTexture("SsaoBlur"))),
		ptrSSAOMaterial(renderer.GetMaterialPool()->GetMaterial("Ssao")),
		ptrSSAOHBlurMaterial(renderer.GetMaterialPool()->GetMaterial("SsaoBlurHorizontal")),
		ptrSSAOVBlurMaterial(renderer.GetMaterialPool()->GetMaterial("SsaoBlurVertical")),
		ptrRenderer(&renderer)
	{

	}

	void SSAOMap::Draw(uint32_t blurCount)
	{
		ID3D12GraphicsCommandList* pList = ptrRenderer->GetCommond()->GetCmdList();
		pList->RSSetViewports(1, &ViewPort);
		pList->RSSetScissorRects(1, &Rect);
		DrawSSAOMap();
		for(uint32_t index = 0; index < blurCount; ++index)
		{
			HorizontalBlur(pList);
			VerticalBlur(pList);
		}
	}

	void SSAOMap::DrawSSAOMap()
	{
		DrawInstance( *ptrSSAOMaterial, *ptrSSAOMap);
		ptrSSAOMap->SetStateFromRenderTargetToShaderResource(ptrRenderer->GetCommond()->GetCmdList());
	}

	void SSAOMap::HorizontalBlur(ID3D12GraphicsCommandList* pList)
	{
		DrawInstance(*ptrSSAOHBlurMaterial, *ptrSSAOBlurMap);
		ptrSSAOBlurMap->SetStateFromRenderTargetToShaderResource(pList);
		ptrSSAOMap->SetStateFromShaderResourceToRenderTarget(pList);
	}

	void SSAOMap::VerticalBlur(ID3D12GraphicsCommandList* pList)
	{
		DrawInstance(*ptrSSAOVBlurMaterial, *ptrSSAOMap);
		ptrSSAOMap->SetStateFromRenderTargetToShaderResource(pList);
		ptrSSAOBlurMap->SetStateFromShaderResourceToRenderTarget(pList);
	}

	void SSAOMap::DrawInstance(const Material& material, const RenderTexture& renderTarget)
	{
		ID3D12GraphicsCommandList* pList = ptrRenderer->GetCommond()->GetCmdList();
		DescriptorPool* pDescriptorPool = ptrRenderer->GetDescPool();
		auto& RTV = pDescriptorPool->GetRtvStart().Offset(renderTarget.GetRtvIndex(), pDescriptorPool->GetRtvDescSize());
		pList->OMSetRenderTargets(1, &RTV.CpuHandle(), false, nullptr);
		pList->ClearRenderTargetView(RTV.CpuHandle(), DirectX::Colors::LightSteelBlue, 0, nullptr);
		auto& SsaoBlurShader = material.GetShader();
		pList->SetGraphicsRootSignature(SsaoBlurShader->GetRootSignature().GetSignature());
		pList->SetPipelineState(SsaoBlurShader->GetPSO().GetPSO());
		pList->IASetVertexBuffers(0, 0, nullptr);
		pList->IASetIndexBuffer(nullptr);
		pList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		SsaoBlurShader->SetRootSignature(pList, pDescriptorPool, material, *ptrRenderer->GetFrameCBuffer());
		pList->DrawInstanced(6, 1, 0, 0);
	}
}
