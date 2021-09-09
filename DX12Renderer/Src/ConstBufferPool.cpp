#include "ConstBufferPool.h"
#include "MathHelper.h"
#include "Camera.h"
#include "TexturePool.h"
#include "Texture.h"
#include "ConstantBuffer.h"
#include <string_view>
#include <DirectXColors.h>
namespace rdr
{
	ConstBufferPool::ConstBufferPool(const RendererFacade& renderer)
	{
		lightPool = std::make_unique<LightPool>();
		ConstantBuffer<ObjectConstant> object(renderer);
		this->objConstBufferVec.emplace_back(std::move(object));
		ConstantBuffer<PassConstant> shadowPass(renderer);
		this->passConstBufferVec.emplace_back(std::move(shadowPass));
		ConstantBuffer<PassConstant> mainPass(renderer);
		this->passConstBufferVec.emplace_back(std::move(mainPass));
#ifdef POINTSHADOW
		ConstantBuffer<PassConstant> pointRight(renderer);
		this->passConstBufferVec.emplace_back(std::move(pointRight));
		ConstantBuffer<PassConstant> pointLeft(renderer);
		this->passConstBufferVec.emplace_back(std::move(pointLeft));
		ConstantBuffer<PassConstant> pointForward(renderer);
		this->passConstBufferVec.emplace_back(std::move(pointForward));
		ConstantBuffer<PassConstant> pointBack(renderer);
		this->passConstBufferVec.emplace_back(std::move(pointBack));
		ConstantBuffer<PassConstant> pointUp(renderer);
		this->passConstBufferVec.emplace_back(std::move(pointUp));
		ConstantBuffer<PassConstant> pointDown(renderer);
		this->passConstBufferVec.emplace_back(std::move(pointDown));
#endif
	}

	uint32_t ConstBufferPool::AddMaterial(const std::string_view& meshName, const RendererFacade& renderer)
	{
		MaterialConstant mat;
		std::string_view tempName = meshName;
		for (size_t length = tempName.size(), i = 0, count = 0; i < length; ++i)
		{
			if (tempName[i] == '_') ++count;
			if (count == 4)
			{
				tempName = tempName.substr(i + 1);
				break;
			}
		}
		if (tempName.substr(0, 6) == "fabric")
		{
			mat.diffuseIndex = renderer.GetTexPool()->GetDiffuseTex(static_cast<std::string>(tempName))->GetSrvIndex();
			mat.normalIndex = renderer.GetTexPool()->GetNormalTex("fabric")->GetSrvIndex();
		}
		else
		{
			mat.diffuseIndex = renderer.GetTexPool()->GetDiffuseTex(static_cast<std::string>(tempName))->GetSrvIndex();
			mat.normalIndex = renderer.GetTexPool()->GetNormalTex(static_cast<std::string>(tempName))->GetSrvIndex();
		}
		ConstantBuffer<MaterialConstant> cbuffer(renderer, mat);
		matConstBufferVec.push_back(std::move(cbuffer));
		return static_cast<uint32_t>(matConstBufferVec.size() - 1);
	}

	uint32_t ConstBufferPool::AddMaterial(ConstantBuffer<MaterialConstant>& mat)
	{
		matConstBufferVec.push_back(std::move(mat));
		return static_cast<uint32_t>(matConstBufferVec.size() - 1);
	}

	void ConstBufferPool::Update(const Camera& camera)
	{
		//更新object常量数据
		{
			auto view = XMMatrixLookAtLH(-5000 * XMLoadFloat4(&LightPool::SunDirection), { 0, 0, 0, 1 }, { 0, 1, 0, 0 });
			auto proj = XMMatrixOrthographicLH(global_lightProjectionWidth, global_lightProjectionHeight, camera.GetNearZ(), camera.GetFarZ());
			auto viewProj = view * proj;
			PassConstant shadow;
			XMStoreFloat4x4(&shadow.viewProj, XMMatrixTranspose(viewProj));
			passConstBufferVec[0].Update(shadow);
		}

		//更新pass常量数据
		{
			auto smmodel = XMLoadFloat4x4(&MathHelper::Identity4x4());
			auto smview = XMMatrixLookAtLH(-5000 * XMLoadFloat4(&LightPool::SunDirection), { 0, 0, 0, 1 }, { 0, 1, 0, 0 });
			auto smproj = XMMatrixOrthographicLH(global_lightProjectionWidth, global_lightProjectionHeight, camera.GetNearZ(), camera.GetFarZ());
			auto shadowMVP = smmodel * smview * smproj * XMLoadFloat4x4(&MathHelper::ShadowTexMatrix());
			auto view = camera.GetViewMatrix();
			auto proj = camera.GetProjMatrix();
			auto viewProj = view * proj;

			PassConstant pass;
			auto pointNum = lightPool->GetPointNum();
			pass.lights[0] = lightPool->GetDirectionalLight(0);
			for (int i = 0; i < pointNum; ++i)
				pass.lights[i + 1] = lightPool->GetPointLight(i);
#ifdef POINTSHADOW
			auto model = XMLoadFloat4x4(&MathHelper::Identity4x4());
			auto pointRightView = XMMatrixLookAtLH(XMLoadFloat3(&lightPool->GetPointLight(pointNum - 1).position), { 20, 600, 0, 1 }, { 0, 1, 0, 0 });
			auto pointRightProj = camera.GetPointProjMatrix();
			auto rightMVP = model * pointRightView * pointRightProj * XMLoadFloat4x4(&MathHelper::ShadowTexMatrix());

			auto pointLeftView = XMMatrixLookAtLH(XMLoadFloat3(&lightPool->GetPointLight(pointNum - 1).position), { -20, 600, 0, 1 }, { 0, 1, 0, 0 });
			auto poinLeftProj = camera.GetPointProjMatrix();
			auto leftMVP = model * pointLeftView * pointRightProj * XMLoadFloat4x4(&MathHelper::ShadowTexMatrix());

			auto pointForwardView = XMMatrixLookAtLH(XMLoadFloat3(&lightPool->GetPointLight(pointNum - 1).position), { 0, 600, 20, 1 }, { 0, 1, 0, 0 });
			auto poinFowardProj = camera.GetPointProjMatrix();
			auto forwardMVP = model * pointForwardView * poinFowardProj * XMLoadFloat4x4(&MathHelper::ShadowTexMatrix());

			auto pointBackView = XMMatrixLookAtLH(XMLoadFloat3(&lightPool->GetPointLight(pointNum - 1).position), { 0, 600, -20, 1 }, { 0, 1, 0, 0 });
			auto poinBackProj = camera.GetPointProjMatrix();
			auto backMVP = model * pointBackView * poinBackProj * XMLoadFloat4x4(&MathHelper::ShadowTexMatrix());

			auto pointUpView = XMMatrixLookAtLH(XMLoadFloat3(&lightPool->GetPointLight(pointNum - 1).position), { 0, 620, 0, 1 }, { 0, 1, 0, 0 });
			auto poinUpProj = camera.GetPointProjMatrix();
			auto upMVP = model * pointUpView * poinUpProj * XMLoadFloat4x4(&MathHelper::ShadowTexMatrix());

			auto pointDownView = XMMatrixLookAtLH(XMLoadFloat3(&lightPool->GetPointLight(pointNum - 1).position), { 0, 580, 0, 1 }, { 0, 1, 0, 0 });
			auto poinDownProj = camera.GetPointProjMatrix();
			auto downMVP = model * pointDownView * poinDownProj * XMLoadFloat4x4(&MathHelper::ShadowTexMatrix());
			XMStoreFloat4x4(&pass.pointShadowMVPRight, XMMatrixTranspose(rightMVP));
			XMStoreFloat4x4(&pass.pointShadowMVPLeft, XMMatrixTranspose(leftMVP));
			XMStoreFloat4x4(&pass.pointShadowMVPForward, XMMatrixTranspose(forwardMVP));
			XMStoreFloat4x4(&pass.pointShadowMVPBack, XMMatrixTranspose(backMVP));
			XMStoreFloat4x4(&pass.pointShadowMVPUp, XMMatrixTranspose(upMVP));
			XMStoreFloat4x4(&pass.pointShadowMVPDown, XMMatrixTranspose(downMVP));
#endif
			XMStoreFloat4x4(&pass.viewProj, XMMatrixTranspose(viewProj));
			XMStoreFloat4x4(&pass.shadowMVP, XMMatrixTranspose(shadowMVP));
			pass.eyePosition = camera.GetCameraPosition();
			passConstBufferVec[1].Update(pass);
		}

#ifdef POINTSHADOW
		auto proj = camera.GetPointProjMatrix();
		{
			auto view = XMMatrixLookAtLH(XMLoadFloat3(&LightPool::MainPointPosition), { 20, 600, 0, 1 }, { 0, 1, 0, 0 });
			auto rightViewProj = view * proj;
			PassConstant right;
			XMStoreFloat4x4(&right.viewProj, XMMatrixTranspose(rightViewProj));
			passConstBufferVec[2].Update(right);
		}

		{
			auto view = XMMatrixLookAtLH(XMLoadFloat3(&LightPool::MainPointPosition), { -20, 600, 0, 1 }, { 0, 1, 0, 0 });
			auto leftViewProj = view * proj;
			PassConstant left;
			XMStoreFloat4x4(&left.viewProj, XMMatrixTranspose(leftViewProj));
			passConstBufferVec[3].Update(left);
		}

		{
			auto view = XMMatrixLookAtLH(XMLoadFloat3(&LightPool::MainPointPosition), { 0, 600, 20, 1 }, { 0, 1, 0, 0 });
			auto forwardViewProj = view * proj;
			PassConstant forward;
			XMStoreFloat4x4(&forward.viewProj, XMMatrixTranspose(forwardViewProj));
			passConstBufferVec[4].Update(forward);
		}

		{
			auto view = XMMatrixLookAtLH(XMLoadFloat3(&LightPool::MainPointPosition), { 0, 600, -20, 1 }, { 0, 1, 0, 0 });
			auto backViewProj = view * proj;
			PassConstant back;
			XMStoreFloat4x4(&back.viewProj, XMMatrixTranspose(backViewProj));
			passConstBufferVec[5].Update(back);
		}

		{
			float tempfovAngleY = 0.5f * MathHelper::Pi;
			float tempaspect = 1;
			float tempnearZ = 1;
			float tempfarZ = 1500;
			auto tempproj = XMMatrixPerspectiveFovLH(tempfovAngleY, tempaspect, tempnearZ, tempfarZ);
			XMFLOAT3 pos = { 0, 1, 0 };
			auto view = XMMatrixLookAtLH(XMLoadFloat3(&pos), { 0, 2, 0, 1 }, { 0, 1, 0, 0 });
			auto upViewProj = view * tempproj;
			PassConstant up;
			XMStoreFloat4x4(&up.viewProj, XMMatrixTranspose(upViewProj));
			passConstBufferVec[6].Update(up);
		}

		{
			float tempfovAngleY = 0.5f * MathHelper::Pi;
			float tempaspect = 1;
			float tempnearZ = 5;
			float tempfarZ = 1500;
			auto tempproj = XMMatrixPerspectiveFovLH(tempfovAngleY, tempaspect, tempnearZ, tempfarZ);
			XMFLOAT3 pos = { 0, 1, 0 };
			auto view = XMMatrixLookAtLH(XMLoadFloat3(&pos), { 0, 0, 0, 1 }, { 0, 1, 0, 0 });
			auto downViewProj = view * tempproj;
			PassConstant down;
			XMStoreFloat4x4(&down.viewProj, XMMatrixTranspose(downViewProj));
			passConstBufferVec[7].Update(down);
		}
#endif
	}

	ConstBufferPool::~ConstBufferPool()
	{

	}
}