#include "Shader.h"
#include "ConstantBuffer.h"
#include "RendererHelper.h"
#include "RootSignature.h"
#include "DescriptorPool.h"
#include "FrameConstBuffer.h"
#include "Texture.h"
#include "Material.h"
#include "PSO.h"

namespace rdr
{

	const std::string Shader::VSTarget = "vs_5_1";
	const std::string Shader::PSTarget = "ps_5_1";

	Shader& Shader::CompileVertexShader(const std::wstring& filename, const std::string& entrypoint)
	{
		pVertexBlob = CompileShader(filename, nullptr, entrypoint, VSTarget);
		return *this;
	}

	Shader& Shader::CompilePixelShader(const std::wstring& filename, const std::string& entrypoint)
	{
		pPixelBlob = CompileShader(filename, nullptr, entrypoint, PSTarget);
		return *this;
	}

	//TODO:这个暂时先这么写，后面到实例化的时候再改
	Shader& Shader::AddInputElement(const std::vector<std::pair<std::string, int32_t>>& ElementData)
	{
		const size_t length = ElementData.size();
		for(int index = 0; index < length; ++index)
		{
			DXGI_FORMAT format;
			switch (ElementData[index].second)
			{
			case 1: format = DXGI_FORMAT_R32_FLOAT; break;
			case 2: format = DXGI_FORMAT_R32G32_FLOAT; break;
			case 3: format = DXGI_FORMAT_R32G32B32_FLOAT; break;
			case 4: format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
			default: DX_THROW("Wrong Demension");
			}
			const D3D12_INPUT_ELEMENT_DESC desc =
			{
				ElementData[index].first.c_str(),
				0,
				format,
				0,
				index == 0 ? 0 : inputElementVec[index - 1].AlignedByteOffset + ElementData[index - 1].second * 4,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
			};
			inputElementVec.push_back(desc);
		}
		return *this;
	}

	Shader& Shader::AddSlot(const SlotInfo& InSlotInfo)
	{
		slotInfoVec.emplace_back(InSlotInfo);
		return *this;
	}

	Shader& Shader::CreateRootSignature(ID3D12Device* pDevice)
	{
		auto slotNum = slotInfoVec.size();
		pRootSig = std::make_unique<RootSignature>(slotNum);

		std::vector<CD3DX12_DESCRIPTOR_RANGE1*> tempRangeVec(slotNum, nullptr);
		for (int i = 0; i < slotNum; ++i)
		{
			if (slotInfoVec[i].type == DescriptorHeapSRV || slotInfoVec[i].type == DescriptorHeapSampler)
			{
				uint32_t tableLength = slotInfoVec[i].tableSize;
				tempRangeVec[i] = new CD3DX12_DESCRIPTOR_RANGE1[tableLength];
				if (slotInfoVec[i].type == DescriptorHeapSampler)
					tempRangeVec[i][0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, slotInfoVec[i].tableSize, slotInfoVec[i].registerPosition, slotInfoVec[i].spacePosition);
				else
					tempRangeVec[i][0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, slotInfoVec[i].tableSize, slotInfoVec[i].registerPosition, slotInfoVec[i].spacePosition);
				(*pRootSig)[i].InitAsDescriptorTable(1, tempRangeVec[i]);
			}
			else
			{
				(*pRootSig)[i].InitAsConstantBufferView(slotInfoVec[i].registerPosition, slotInfoVec[i].spacePosition);
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

		return *this;
	}

	void Shader::CreatePSO(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& InDesc, ID3D12Device4* device)
	{
		ptrPSO = std::make_unique<PipelineStateObject>(InDesc);
		ptrPSO->SetRootSignature(pRootSig->GetSignature());
		ptrPSO->SetInputLayout({ inputElementVec.data(), static_cast<uint32_t>(inputElementVec.size()) });
		ptrPSO->SetVertexShader(CD3DX12_SHADER_BYTECODE(pVertexBlob->GetBufferPointer(), pVertexBlob->GetBufferSize()));
		ptrPSO->SetPixelShader(CD3DX12_SHADER_BYTECODE(pPixelBlob->GetBufferPointer(), pPixelBlob->GetBufferSize()));
		ptrPSO->Create(device);
	}

	void Shader::SetRootSignature(ID3D12GraphicsCommandList* pList, const DescriptorPool* pDescPool, const Material& material, const FrameConstBuffer& frameBuffer)
	{
		int length = static_cast<int>(pRootSig->GetParameterCount());
		uint32_t tTextureIndex = 0;
		uint32_t tSamplerIndex = 0;
		for (int i = 0; i < length; ++i)
		{
			switch (slotInfoVec[i].type)
			{
			case DescriptorHeapSRV:
				pList->SetGraphicsRootDescriptorTable(i, 
					pDescPool->GetSrvStart().Offset(material.GetTexture()[tTextureIndex]->GetSrvIndex(), pDescPool->GetSrvCbvUavDescSize()).GpuHandle());
				++tTextureIndex;
				break;
			case DescriptorHeapSampler:
				pList->SetGraphicsRootDescriptorTable(i, 
					pDescPool->GetSamplerStart().Offset(material.GetSampler()[tSamplerIndex], pDescPool->GetSamplerSize()).GpuHandle());
				++tSamplerIndex;
				break;
			case ObjectCBuffer:
				pList->SetGraphicsRootConstantBufferView(i, material.GetConstBuffer().GetVirtualAddress());
				break;
			case FrameCBuffer:
				pList->SetGraphicsRootConstantBufferView(i, frameBuffer.GetBuffer().GetVirtualAddress());
				break;
			default:
				break;
			}
		}
	}
}
