#include "Pass.h"
#include "DescriptorPool.h"
#include "ConstBufferPool.h"
#include "ConstantBuffer.h"
namespace rdr
{
	Pass::Pass(const std::vector<SlotInfo>& shaderSlots,
		const std::wstring& filePath,
		const std::string& vsEntry,
		const std::string& psEntry,
		ID3D12Device* pDevice)
		: slotInfoVec(shaderSlots)
	{
		pVertexBlob = CompileShader(filePath, nullptr, vsEntry, "vs_5_1");
		pPixelBlob = CompileShader(filePath, nullptr, psEntry, "ps_5_1");

		auto slotNum = shaderSlots.size();
		pRootSig = std::make_unique<RootSignature>(slotNum);

		std::vector<CD3DX12_DESCRIPTOR_RANGE1*> tempRangeVec(slotNum, nullptr);
		for (int i = 0; i < slotNum; ++i)
		{
			if (shaderSlots[i].type == DescriptorHeapSRV || shaderSlots[i].type == DescriptorHeapSampler)
			{
				uint32_t tableLength = shaderSlots[i].tableSize;
				tempRangeVec[i] = new CD3DX12_DESCRIPTOR_RANGE1[tableLength];
				if (shaderSlots[i].type == DescriptorHeapSampler)
					tempRangeVec[i][0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, shaderSlots[i].tableSize, shaderSlots[i].registerPosition, shaderSlots[i].spacePosition);
				else
					tempRangeVec[i][0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, shaderSlots[i].tableSize, shaderSlots[i].registerPosition, shaderSlots[i].spacePosition);
				(*pRootSig)[i].InitAsDescriptorTable(1, tempRangeVec[i]);
			}
			else
			{
				(*pRootSig)[i].InitAsConstantBufferView(shaderSlots[i].registerPosition, shaderSlots[i].spacePosition);
			}
		}

		pRootSig->Create(pDevice, [&tempRangeVec]() ->void
			{
				size_t length = tempRangeVec.size();
				for (size_t i = 0; i < length; ++i)
					if (tempRangeVec[i] != nullptr)
					{
						delete[] tempRangeVec[i];
						tempRangeVec[i] = nullptr;
					}
			});
	}

	void Pass::SetRootParameter(ID3D12GraphicsCommandList* pList, DescriptorPool* pDescPool, ConstBufferPool* pConstPool, uint32_t matIndex)
	{
		int length = static_cast<int>(pRootSig->GetParameterCount());
		for (int i = 0; i < length; ++i)
		{
			switch (slotInfoVec[i].type)
			{
			case DescriptorHeapSRV:
				pList->SetGraphicsRootDescriptorTable(i, pDescPool->GetSrvStart().Offset(slotInfoVec[i].viewIndex, pDescPool->GetSrvCbvUavDescSize()).GpuHandle());
				break;
			case DescriptorHeapSampler:
				pList->SetGraphicsRootDescriptorTable(i, pDescPool->GetSamplerStart().Offset(slotInfoVec[i].viewIndex, pDescPool->GetSamplerSize()).GpuHandle());
				break;
			case ConstantBufferObj:
				pList->SetGraphicsRootConstantBufferView(i, pConstPool->GetObjCBuffer(slotInfoVec[i].constBufferIndex).GetVirtualAddress());
				break;
			case ConstantBufferPass:
				pList->SetGraphicsRootConstantBufferView(i, pConstPool->GetPassCBuffer(slotInfoVec[i].constBufferIndex).GetVirtualAddress());
				break;
			case ConstantBufferMat:
				pList->SetGraphicsRootConstantBufferView(i, pConstPool->GetMatCBuffer(matIndex).GetVirtualAddress());
				break;
			default:
				break;
			}
		}
	}

	void Pass::SetSRVTable(ID3D12GraphicsCommandList* pList, DescriptorPool* pDescPool)
	{

	}

	void Pass::SetSamplerTable(ID3D12GraphicsCommandList* pList, DescriptorPool* pDescPool)
	{

	}
}