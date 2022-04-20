#pragma once
#include <DirectXPackedVector.h>
#include <string>
#include <vector>
#include "ConstValue.h"
#include "Texture.h"

namespace rdr
{
	using namespace DirectX;

	class HeapManager;
	class Renderer;

	class CommonTexture final : public Texture
	{
	public:
		CommonTexture(const std::string& InName);
		~CommonTexture() override = default;

	public:
		static PackedVector::XMCOLOR* GetRandomData(uint32_t width, uint32_t height);
		void CreateTextureFromFlie(const std::wstring& filePath, const Renderer& renderer, bool is2D);
		void CreateRandomTexture(const Renderer& renderer, DXGI_FORMAT format, uint32_t width, uint32_t height, bool is2D);

	public:
		static constexpr DXGI_FORMAT RandomVectorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	};
}
