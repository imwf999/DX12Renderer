#include "Material.h"
#include "RendererHelper.h"
#include "ConstantBuffer.h"
#include "TexturePool.h"

namespace rdr
{
	using namespace DirectX;

	void Material::CreateConstBuffer(const Renderer& renderer, const std::vector<std::pair<std::string, std::any>>& InBufferData)
	{
		using Element = CBufferElement;
		size_t tLength = InBufferData.size();
		std::vector<void*> tValuePtrVec(tLength, nullptr);
		const auto& getType = [&tValuePtrVec](const std::any& InValue, int32_t index)->CBufferElement::Type
		{
			const type_info& tType = InValue.type();
			if (tType == typeid(float)) { tValuePtrVec[index] =  new float(std::any_cast<float>(InValue));  return Element::Float; }
			if (tType == typeid(XMFLOAT2)) { tValuePtrVec[index] = new XMFLOAT2(std::any_cast<XMFLOAT2>(InValue)); return Element::Float2; }
			if (tType == typeid(XMFLOAT3)) { tValuePtrVec[index] = new XMFLOAT3(std::any_cast<XMFLOAT3>(InValue)); return Element::Float3; }
			if (tType == typeid(XMFLOAT4)) { tValuePtrVec[index] = new XMFLOAT4(std::any_cast<XMFLOAT4>(InValue)); return Element::Float4; }
			if (tType == typeid(XMFLOAT4X4)) { tValuePtrVec[index] = new XMFLOAT4X4(std::any_cast<XMFLOAT4X4>(InValue)) ; return Element::Matrix4x4; }
			if (tType == typeid(uint32_t)) { tValuePtrVec[index] = new uint32_t(std::any_cast<uint32_t>(InValue)); return Element::UInt; }
			if (tType == typeid(int32_t)) { tValuePtrVec[index] = new int32_t(std::any_cast<int32_t>(InValue)); return Element::Int; }
			if (tType == typeid(XMINT2)) { tValuePtrVec[index] = new XMINT2(std::any_cast<XMINT2>(InValue)); return Element::Int2; }
			if (tType == typeid(XMINT3)) { tValuePtrVec[index] = new XMINT3(std::any_cast<XMINT3>(InValue)); return Element::Int3; }
			if (tType == typeid(XMINT4)) { tValuePtrVec[index] = new XMINT4(std::any_cast<XMINT4>(InValue)); return Element::Int4; }
			if (tType == typeid(bool)) { tValuePtrVec[index] = new bool(std::any_cast<bool>(InValue)); return Element::Bool; }
			throw "Type Not Supported";
		};

		uint32_t tPreElementOffset = 0;
		for(size_t index = 0; index < tLength; ++index)
		{
			Element element;
			element.name = InBufferData[index].first;
			element.type = getType(InBufferData[index].second, index);
			element.size = Element::TypeSize(element.type);
			element.offset = tPreElementOffset;
			tPreElementOffset += UpperAlignment(element.size, 4);
			CBufferElementMap.insert({ element.name, element });
			CBufferElementVec.push_back(element);
		}
		ptrConstBuffer = std::make_shared<ConstantBuffer>(name);
		ptrConstBuffer->CreateConstBuffer(renderer, tPreElementOffset);
		for(int32_t index = 0; index < tLength; ++index)
		{
			if (tValuePtrVec[index] == nullptr) throw "Data Is Null";
			const Element& tElement = CBufferElementMap[InBufferData[index].first];
			ptrConstBuffer->Update(tElement.offset, tValuePtrVec[index], tElement.size);
		}
		for(uint32_t index = 0; index < tLength; ++index)
		{
			if (tValuePtrVec[index] != nullptr)
				delete tValuePtrVec[index];
		}
	}

	void Material::BindTexture(const std::string& InName, const Renderer& renderer)
	{
		const auto& ptrTex = renderer.GetTexPool()->GetTexture(InName);
		texVec.push_back(ptrTex);
	}
}
