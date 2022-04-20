#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <any>

namespace rdr
{
	class Shader;
	class Texture;
	class ConstantBuffer;
	class Renderer;

	struct CBufferElement
	{
		enum Type;

		std::string name;
		Type type;
		uint32_t size;
		uint32_t offset;

		enum Type
		{
			Float,
			Float2,
			Float3,
			Float4,
			Matrix4x4,
			UInt,
			Int,
			Int2,
			Int3,
			Int4,
			Bool
		};

		uint32_t static TypeSize(Type type)
		{
			switch (type) 
			{
			case Float: return 4;
			case Float2: return 8;
			case Float3: return 12;
			case Float4: return 16;
			case Matrix4x4: return 64;
			case UInt: return 4;
			case Int: return 4;
			case Int2: return 8;
			case Int3: return 12;
			case Int4: return 16;
			case Bool: return 1;
			default: return 0;
			}
		}
	};

	class Material
	{
	public:
		Material(const std::string& InName, const std::shared_ptr<Shader>& InPtrShader) : name(InName), ptrShader(InPtrShader) {  }
		Material(const Material& mat) = delete;
		Material(Material&& mat) = delete;
		~Material() = default;

	public:
		void CreateConstBuffer(const Renderer& renderer, const std::vector<std::pair<std::string, std::any>>& InBufferData);
		void BindTexture(const std::string& InName, const Renderer& renderer);
		void BindSampler(uint32_t samplerIndexInDescHeap) { samplerVec.push_back(samplerIndexInDescHeap); }
		const std::string& GetName() const { return name; }
		ConstantBuffer& GetConstBuffer() { return *ptrConstBuffer; }
		const ConstantBuffer& GetConstBuffer() const { return *ptrConstBuffer; }
		const std::vector<std::shared_ptr<Texture>>& GetTexture() const { return texVec; }
		const std::vector<uint32_t>& GetSampler() const { return samplerVec; }
		const std::shared_ptr<Shader>& GetShader() const { return ptrShader; }
		const std::vector<CBufferElement>& GetCBufferElementVec() const { return CBufferElementVec; }

	private:
		std::string name;
		std::unordered_map<std::string, CBufferElement> CBufferElementMap;		//这个是为了通过字符串访问
		std::vector<CBufferElement> CBufferElementVec;		//这个是为了按照shader中的cbuffer顺序访问
		std::shared_ptr<ConstantBuffer> ptrConstBuffer;
		std::shared_ptr<Shader> ptrShader;
		std::vector<std::shared_ptr<Texture>> texVec;
		std::vector<uint32_t> samplerVec;
	};
}
