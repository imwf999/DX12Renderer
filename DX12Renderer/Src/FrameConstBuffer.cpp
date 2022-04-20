#include "FrameConstBuffer.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "Game.h"
#include "Shadow.h"

namespace rdr
{
	FrameConstBuffer::FrameConstBuffer(const Renderer& renderer)
	{
		ptrCamera = Game::Instance()->GetCamera();
		ptrCBuffer = std::make_unique<ConstantBuffer>("CommonFrameCBuffer");
		ptrCBuffer->CreateConstBuffer(renderer, data);
		Update();
	}

	FrameConstBuffer::~FrameConstBuffer()
	{
	}

	void FrameConstBuffer::Update()
	{
		const XMMATRIX& view = ptrCamera->GetViewMatrix();
		const XMMATRIX& proj = ptrCamera->GetProjMatrix();
		const XMMATRIX& viewProj = view * proj;
		const XMMATRIX& shadowViewProj = Shadow::ShadowViewMatrix() * Shadow::ShadowProjMatrix();
		const XMMATRIX& projTex = proj * XMLoadFloat4x4(&Math::TextureMatrix());
		XMVECTOR tempProj = XMMatrixDeterminant(proj);
		const XMMATRIX& InvProj = XMMatrixInverse(&tempProj, proj);
		const XMMATRIX& shadowMVP = Shadow::ShadowMVPMatrix();
		XMStoreFloat4x4(&data.ViewMatrix, XMMatrixTranspose(view));
		XMStoreFloat4x4(&data.ProjMatrix, XMMatrixTranspose(proj));
		XMStoreFloat4x4(&data.ViewProjMatrix, XMMatrixTranspose(viewProj));
		XMStoreFloat4x4(&data.ShadowViewProjMatrix, XMMatrixTranspose(shadowViewProj));
		XMStoreFloat4x4(&data.ProjTextureMatrix, XMMatrixTranspose(projTex));
		XMStoreFloat4x4(&data.InverseProjMatrix, XMMatrixTranspose(InvProj));
		XMStoreFloat4x4(&data.ShadowMVPMatrix, XMMatrixTranspose(shadowMVP));
		const XMFLOAT3& eyePos = ptrCamera->GetCameraPosition();
		data.EyePostion = { eyePos.x, eyePos.y, eyePos.z, 1.0f };
		ptrCBuffer->Update(data);
	}
}
