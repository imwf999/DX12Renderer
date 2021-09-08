#pragma once
#include "IResource.h"
#include "RendererHelper.h"
#include "RendererFacade.h"
#include "HeapResourceManager.h"
#include "MathHelper.h"
#include <DirectXMath.h>

namespace rdr
{
	struct Light
	{
		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		float fallOffStart = 100.0f;
		DirectX::XMFLOAT4 direction = global_directionalLightDirection;
		DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
		float fallOffEnd = 400.0f;
	};

	struct ObjectConstant
	{
		DirectX::XMFLOAT4X4 objToWold = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 normalObjToWorld = MathHelper::Identity4x4();
	};

	struct PassConstant
	{
		DirectX::XMFLOAT4X4 viewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 shadowMVP = MathHelper::Identity4x4();
#ifdef POINTSHADOW
		DirectX::XMFLOAT4X4 pointShadowMVPRight = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 pointShadowMVPLeft = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 pointShadowMVPForward = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 pointShadowMVPBack = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 pointShadowMVPUp = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 pointShadowMVPDown = MathHelper::Identity4x4();
#endif
		DirectX::XMFLOAT3 eyePosition = { 0, 0, 0 };
#ifdef POINTSHADOW
		uint32_t pointLightNum = 9;
		Light lights[10];
#else
		uint32_t pointLightNum = 8;
		Light lights[9];
#endif
	};

	struct MaterialConstant
	{
		DirectX::XMFLOAT4 diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT4 ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
		DirectX::XMFLOAT4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT4 emissive = { 0.0f, 0.0f, 0.0f, 1.0f };		//自发光
		DirectX::XMFLOAT4 transparent = { 0.0f, 0.0f, 0.0f, 1.0f };		//透明颜色
		DirectX::XMFLOAT4 reflective = { 0.0f, 0.0f, 0.0f, 0.0f };			//反射光
		DirectX::XMFLOAT3 fresnelR0 = { 0.1f, 0.1f, 0.1f };
		float shininess = 0.98f;
		float bumpScaling = 0.0f;
		float reflectivity = 0.0f;
		uint32_t diffuseIndex = 0;			//纹理贴图索引
		uint32_t normalIndex = 0;			//法线贴图索引
	};

	template<typename Ty>
	class ConstantBuffer : public IResource
	{
	public:
		ConstantBuffer(const RendererFacade& renderer, const Ty& value = Ty())
		{
			constBufferSize = UpperAlignment(sizeof(Ty), 256);
			resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(constBufferSize);
			renderer.GetResMgr()->CreateBufferResourceInUploadHeap(&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, pResource, constBufferSize);
			pConstValue = nullptr;
			ThrowIfFailed(pResource->Map(0, nullptr, reinterpret_cast<void**>(&pConstValue)));
			memcpy(pConstValue, &value, constBufferSize);
		}
		ConstantBuffer(ConstantBuffer&& buffer) noexcept
		{
			this->pConstValue = buffer.pConstValue;
			this->constBufferSize = buffer.constBufferSize;
			this->pResource = std::move(buffer.pResource);
			this->virtualAddress = buffer.virtualAddress;
			this->resourceDesc = buffer.resourceDesc;
			buffer.pResource = nullptr;
			buffer.pConstValue = nullptr;
		}
		~ConstantBuffer()
		{
			if (pResource != nullptr)
				pResource->Unmap(0, nullptr);
		}
		void Update(const Ty& constValue)
		{
			memcpy(pConstValue, &constValue, constBufferSize);
		}
		const Ty* GetConstPtr() const { return pConstValue; }

	private:
		Ty* pConstValue;
		uint32_t constBufferSize;
	};
}