#include "CommonTexture.h"
#include "DescriptorPool.h"
#include "RendererHelper.h"
#include "DDSTextureLoader12.h"
#include "HeapManager.h"
#include "Renderer.h"
#include "Display.h"
#include "Commond.h"
#include <random>

namespace rdr
{
	CommonTexture::CommonTexture(const std::string& InName)
	{
		name = InName;
	}

	void CommonTexture::CreateTextureFromFlie(const std::wstring& filePath, const Renderer& renderer, bool is2D)
	{
		std::unique_ptr<uint8_t[]> ddsData;
		std::vector<D3D12_SUBRESOURCE_DATA> subResourceVec;
		ThrowIfFailed(LoadDDSTextureFromFile(renderer.GetDevice(), filePath.c_str(), ddsData, subResourceVec, this->resourceDesc));
		renderer.GetResMgr()->CreateTextureInDefaultHeap(this, renderer.GetCommond()->GetCmdList(), subResourceVec);
		CreateSRV(renderer, resourceDesc.Format, resourceDesc.MipLevels, is2D);
	}

	void CommonTexture::CreateRandomTexture(const Renderer& renderer, DXGI_FORMAT format, uint32_t width, uint32_t height, bool is2D)
	{
		if(is2D == true)
		{
			SetResourceDescAsTexture(format, width, height, D3D12_RESOURCE_FLAG_NONE);

			PackedVector::XMCOLOR* ptrRandomData = GetRandomData(width, height);

			std::vector<D3D12_SUBRESOURCE_DATA> subResourceVec;
			D3D12_SUBRESOURCE_DATA subResourceData = {};
			subResourceData.pData = ptrRandomData;
			subResourceData.RowPitch = height * sizeof(PackedVector::XMCOLOR);
			subResourceData.SlicePitch = subResourceData.RowPitch * width;
			subResourceVec.push_back(subResourceData);

			renderer.GetResMgr()->CreateTextureInDefaultHeap(this, renderer.GetCommond()->GetCmdList(), subResourceVec);
			CreateSRV(renderer, format);
			if (ptrRandomData != nullptr) delete[] ptrRandomData;
		}
		else
		{
			//TODO:
		}
	}


	PackedVector::XMCOLOR* CommonTexture::GetRandomData(uint32_t width, uint32_t height)
	{
		PackedVector::XMCOLOR* initData = new PackedVector::XMCOLOR[width * height];

		std::default_random_engine engine;
		std::uniform_real_distribution distribution(0.0f, 1.0f);
		for (uint32_t i = 0; i < height; ++i)
		{
			for (uint32_t j = 0; j < width; ++j)
			{
				XMFLOAT3 v(distribution(engine), distribution(engine), distribution(engine));

				initData[i * height + j] = PackedVector::XMCOLOR(v.x, v.y, v.z, 0.0f);
			}
		}
		return initData;
	}

}
