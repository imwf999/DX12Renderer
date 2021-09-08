#pragma once
#include <memory>
#include <vector>
#include "RootSignature.h"
namespace rdr
{
	class DescriptorPool;
	class ConstBufferPool;
	class RootSignature;

	class Pass
	{
	public:
		enum ParameterType;
		struct SlotInfo;
	public:
		Pass(const std::vector<SlotInfo>& shaderSlots,
			const std::wstring& filePath,
			const std::string& vsEntry,
			const std::string& psEntry,
			ID3D12Device* pDevice);
		Pass(Pass&& pass) noexcept
		{
			pVertexBlob = std::move(pass.pVertexBlob);
			pPixelBlob = std::move(pass.pPixelBlob);
			pRootSig = std::move(pass.pRootSig);
			slotInfoVec = std::move(pass.slotInfoVec);
			pass.pVertexBlob = nullptr;
			pass.pPixelBlob = nullptr;
			pass.pRootSig = nullptr;
		}
		~Pass() {  }

	public:
		ID3D12RootSignature* GetRootSignature() const { return pRootSig->GetSignature(); }
		BYTE* GetVertexShaderCode() const { return reinterpret_cast<BYTE*>(pVertexBlob->GetBufferPointer()); }
		SIZE_T GetVertexShaderSize() const { return pVertexBlob->GetBufferSize(); }
		BYTE* GetPixelShaderCode() const { return reinterpret_cast<BYTE*>(pPixelBlob->GetBufferPointer()); }
		SIZE_T GetPixelShaderSize() const { return pPixelBlob->GetBufferSize(); }
		void SetRootParameter(ID3D12GraphicsCommandList* pList, DescriptorPool* pDescPool, ConstBufferPool* pConstPool, uint32_t matIndex);
		void SetSRVTable(ID3D12GraphicsCommandList* pList, DescriptorPool* pDescPool);
		void SetSamplerTable(ID3D12GraphicsCommandList* pList, DescriptorPool* pDescPool);
		enum ParameterType : uint32_t
		{
			DescriptorHeapSRV,
			DescriptorHeapSampler,
			ConstantBufferObj,
			ConstantBufferPass,
			ConstantBufferMat
		};
		struct SlotInfo
		{
			ParameterType type;
			uint32_t registerPosition;
			uint32_t spacePosition;
			uint32_t tableSize;
			union
			{
				uint32_t viewIndex;
				uint32_t constBufferIndex;
			};
		};

	private:
		ComPtr<ID3DBlob> pVertexBlob;
		ComPtr<ID3DBlob> pPixelBlob;
		std::unique_ptr<RootSignature> pRootSig;
		std::vector<SlotInfo> slotInfoVec;
	};
}