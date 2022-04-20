#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <memory>
#include <vector>
#include <string>


namespace rdr
{
	using Microsoft::WRL::ComPtr;
	class Material;
	class DescriptorPool;
	class RootSignature;
	class FrameConstBuffer;
	class PipelineStateObject;

	class Shader
	{
		struct SlotInfo;

	public:
		Shader(const std::string& name) : shaderName(name) {  }
		~Shader() = default;

	public:
		const std::string& GetName() const { return shaderName; }
		const PipelineStateObject& GetPSO() const { return *ptrPSO; }
		const RootSignature& GetRootSignature() const { return *pRootSig; }
		Shader& CompileVertexShader(const std::wstring& filename, const std::string& entrypoint);
		Shader& CompilePixelShader(const std::wstring& filename, const std::string& entrypoint);
		Shader& AddInputElement(const std::vector<std::pair<std::string, int32_t>>& ElementData);
		Shader& AddSlot(const SlotInfo& InSlotInfo);
		Shader& CreateRootSignature(ID3D12Device* pDevice);
		void CreatePSO(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& InDesc, ID3D12Device4* device);
		void SetRootSignature(ID3D12GraphicsCommandList* pList, const DescriptorPool* pDescPool, const Material& material, const FrameConstBuffer& frameBuffer);

	public:
		enum ParameterType : uint32_t
		{
			DescriptorHeapSRV,
			DescriptorHeapSampler,
			ObjectCBuffer,
			FrameCBuffer
		};

	private:
		//TODO:如果是向GPU传递常量的话tableSize就没什么用，待修改
		struct SlotInfo
		{
			SlotInfo(ParameterType InType, uint32_t InRegisterPos, uint32_t InSpacePos, uint32_t InTableSize)
				:type(InType), registerPosition(InRegisterPos), spacePosition(InSpacePos), tableSize(InTableSize)
			{ }
			SlotInfo(ParameterType InType, uint32_t InRegisterPos, uint32_t InSpacePos)
				: type(InType), registerPosition(InRegisterPos), spacePosition(InSpacePos), tableSize(0)
			{ }

			ParameterType type;
			uint32_t registerPosition;
			uint32_t spacePosition;
			uint32_t tableSize;
		};

	private:
		std::string shaderName;
		std::vector< D3D12_INPUT_ELEMENT_DESC> inputElementVec;
		ComPtr<ID3DBlob> pVertexBlob;
		ComPtr<ID3DBlob> pPixelBlob;
		std::unique_ptr<RootSignature> pRootSig;
		std::unique_ptr<PipelineStateObject> ptrPSO;
		std::vector<SlotInfo> slotInfoVec;

	private:
		static const std::string VSTarget;
		static const std::string PSTarget;
	};
}
