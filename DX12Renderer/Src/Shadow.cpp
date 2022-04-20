#include "Shadow.h"
#include "Camera.h"
#include "Commond.h"
#include "ConstantBuffer.h"
#include "DescriptorPool.h"
#include "LightPool.h"
#include "Material.h"
#include "MaterialPool.h"
#include "Renderer.h"
#include "RootSignature.h"
#include "Shader.h"
#include "TexturePool.h"
#include "Game.h"
#include "MathHelper.h"
#include "Mesh.h"
#include "PSO.h"

namespace rdr
{
	XMMATRIX Shadow::ViewMatrix;
	XMMATRIX Shadow::ProjMatrix;

	Shadow::Shadow(const Renderer& renderer)
		: ptrMaterial(renderer.GetMaterialPool()->GetMaterial("ShadowMap")),
	ptrDepthTexture(std::dynamic_pointer_cast<DepthTexture>(renderer.GetTexPool()->GetTexture("ShadowMapTex"))),
	ptrRenderer(&renderer)
	{
		ptrShadowCamera = Game::Instance()->GetCamera();
		ViewMatrix = XMMatrixLookAtLH(-5000 * XMLoadFloat4(&LightPool::SunDirection), { 0, 0, 0, 1 }, { 0, 1, 0, 0 });
		ProjMatrix = XMMatrixOrthographicLH(global_lightProjectionWidth, global_lightProjectionHeight, ptrShadowCamera->GetNearZ(), ptrShadowCamera->GetFarZ());
	}

	Shadow::~Shadow() = default;

	void Shadow::DrawShadowMap()
	{
		ID3D12GraphicsCommandList* pList = ptrRenderer->GetCommond()->GetCmdList();
		DescriptorPool* pDescriptorPool = ptrRenderer->GetDescPool();
		const auto& mesh = ptrRenderer->GetMeshVec();
		pList->RSSetViewports(1, &ViewPort);
		pList->RSSetScissorRects(1, &Rect);
		auto& DSV = pDescriptorPool->GetDsvStart().Offset(ptrDepthTexture->GetDsvIndex(), pDescriptorPool->GetDsvDescSize());
		pList->OMSetRenderTargets(0, nullptr, false, &DSV.CpuHandle());
		pList->ClearDepthStencilView(DSV.CpuHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		const auto& shader = ptrMaterial->GetShader();
		pList->SetGraphicsRootSignature(shader->GetRootSignature().GetSignature());
		pList->SetPipelineState(shader->GetPSO().GetPSO());
		uint32_t length = static_cast<uint32_t>(mesh.size()) - 1;
		for(uint32_t index = 0; index < length; ++index)					//最后一个mesh是天空盒，绘制阴影和SSAO时不需要绘制
		{
			mesh[index]->DrawByIndex(pList, pDescriptorPool, *ptrRenderer->GetFrameCBuffer(), *ptrMaterial);
		}
		ptrDepthTexture->SetStateFromDepthWriteToGenericRead(pList);
	}

	const Shader& Shadow::GetShadowMapShader() const
	{
		return *ptrMaterial->GetShader();
	}

	XMMATRIX Shadow::ShadowMVPMatrix()
	{
		const XMMATRIX& model = XMLoadFloat4x4(&Math::Identity4x4());
		return model * ViewMatrix * ProjMatrix * XMLoadFloat4x4(&Math::TextureMatrix());
	}
}
