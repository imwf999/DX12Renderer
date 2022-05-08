#include "MaterialPool.h"
#include "ConstantBuffer.h"
#include "ShaderPool.h"
#include "CommonTexture.h"
#include "TexturePool.h"
#include "Material.h"
#include <DirectXMath.h>

#include "LightPool.h"
#include "MathHelper.h"
#include "RendererHelper.h"

namespace rdr
{
	using namespace DirectX;

	MaterialPool::MaterialPool(const Renderer& InRenderer)
		: renderer(InRenderer)
	{
		using AnyVector = std::vector<std::pair<std::string, std::any>>;

		//TODO:材质和纹理的命名目前很混乱，暂时先这么用着
		const auto& tShadowMapMat = AddMaterial("ShadowMap", "ShadowMap");
		tShadowMapMat->BindCBuffer(renderer, { {"WorldMatrix", Math::Identity4x4() } });

		const auto& tPtrSkyBox = AddMaterial("SkyBox", "SkyBox");
		tPtrSkyBox->BindCBuffer(renderer, { {"WorldMatrix", Math::Identity4x4() } });
		tPtrSkyBox->BindTexture("SkyBox", renderer);
		tPtrSkyBox->BindSampler(0);

		const auto& tPtrSSR = AddMaterial("SSR", "SSR");
		tPtrSSR->BindTexture("GBufferDiffuse", renderer);
		tPtrSSR->BindTexture("ScreenWorldNormal", renderer);
		tPtrSSR->BindTexture("SsaoDepthTex", renderer);		
		tPtrSSR->BindTexture("GBufferDirectLight", renderer);
		tPtrSSR->BindTexture("ShadowMapTex", renderer);
		tPtrSSR->BindTexture("RandomVecTex", renderer);
		tPtrSSR->BindSampler(0);
		tPtrSSR->BindSampler(1);
		tPtrSSR->BindSampler(2);
		tPtrSSR->BindSampler(3);
		const std::vector<std::pair<std::string, std::any>>& tCBufferValue =
		{
			{"WorldMatrix", Math::Identity4x4() },{"NormalWorldMatrix", Math::Identity4x4() }, {"Diffuse", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
			{"Ambient", XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f) }, {"Specular", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }, {"FresnelR0", XMFLOAT3(0.1f, 0.1f, 0.1f) },
			{"Shininess", 0.98f },{"LightColor",  XMFLOAT3(1.0f, 1.0f, 1.0f)},{"FallOffStart", 0.0f }, {"Direction",  LightPool::SunDirection},
			{"PointPos", LightPool::AreaLightPosition }, {"OpenIndirectLight", CacheOpenIndirectLight }
		};
		tPtrSSR->BindCBuffer(renderer, tCBufferValue);

#if SSAO
		const AnyVector& tSsaoCBuffer = { {"OcclusionFadeStart", 2.0f } , {"OcclusionFadeEnd", 10.0f }, {"SurfaceEpsilon", 2.0f }, {"SSAORadius", 15.0f } };
		const auto& tPtrSsaoMat = AddMaterial("Ssao", "Ssao");
		tPtrSsaoMat->BindCBuffer(renderer, tSsaoCBuffer);
		tPtrSsaoMat->BindTexture("ScreenNormal", renderer);
		tPtrSsaoMat->BindTexture("SsaoDepthTex", renderer);
		tPtrSsaoMat->BindTexture("RandomVecTex", renderer);
		tPtrSsaoMat->BindSampler(0);
		tPtrSsaoMat->BindSampler(2);
		tPtrSsaoMat->BindSampler(3);

		const auto& tWeights = Math::CalcGaussWeights(2.5f);
		AnyVector tSsaoBlurCBuffer =
		{
			{"BlurWeights0",XMFLOAT4(&tWeights[0]) }, {"BlurWeights1",  XMFLOAT4(&tWeights[4])}, {"BlurWeights2", XMFLOAT4(&tWeights[8]) },
			{"InvRenderTargetSize", XMFLOAT4(1.0f / global_SSAOMapWidth, 1.0f / global_SSAOMapHeight, 0, 0) }, {"HorizontalBlur", true }
		};
		const auto& tPtrSsaoBlur = AddMaterial("SsaoBlurHorizontal", "SsaoBlur");
		tPtrSsaoBlur->BindCBuffer(renderer, tSsaoBlurCBuffer);
		tPtrSsaoBlur->BindTexture("SsaoTex", renderer);
		tPtrSsaoBlur->BindTexture("ScreenNormal", renderer);
		tPtrSsaoBlur->BindSampler(0);

		tSsaoBlurCBuffer[tSsaoBlurCBuffer.size() - 1].second = false;
		const auto& tPtrSsaoBlurVer = AddMaterial("SsaoBlurVertical", "SsaoBlur");
		tPtrSsaoBlurVer->BindCBuffer(renderer, tSsaoBlurCBuffer);
		tPtrSsaoBlurVer->BindTexture("SsaoBlur", renderer);
		tPtrSsaoBlurVer->BindTexture("ScreenNormal", renderer);
		tPtrSsaoBlurVer->BindSampler(0);
#endif
	}

	MaterialPool::~MaterialPool()
	{
	}

	std::shared_ptr<Material> MaterialPool::AddMaterial(const std::string& InName, const std::string& InShaderName)
	{
		const auto& tIndex = MaterialData.find(InName);
		if (tIndex != MaterialData.cend())
			throw "Two Materials Have The Same Name";
		const std::shared_ptr<Shader>& tPtrShader = renderer.GetShaderPool()->GetShader(InShaderName);
		std::shared_ptr<Material> ptrMat = std::make_shared<Material>(InName, tPtrShader);
		MaterialData.insert({ InName, ptrMat });
		return ptrMat;
	}

	std::vector<std::shared_ptr<Material>> MaterialPool::AddMaterialFromSponzaMesh(const std::string& meshName)
	{
		static uint32_t phongMatNum = 0;

		const std::tuple<std::string, std::string>& texture = GetTextureFromMesh(meshName);
		std::string diffuseName = std::get<0>(texture);
		std::string normalName = std::get<1>(texture);

		if(renderer.GetTexPool()->ContainTexture(diffuseName) == false)
		{
			diffuseName = "DefaultDiffuse";
			normalName = "DefaultNormal";
		}

		const std::string& num = std::to_string(phongMatNum);
		std::string matName = "Material" +num;
		std::string gbufferMatName = "GBufferMaterial" + num;
		phongMatNum++;
		const std::vector<std::pair<std::string, std::any>>& tCBufferValue =
		{
			{"WorldMatrix", Math::Identity4x4() },{"NormalWorldMatrix", Math::Identity4x4() }, {"Diffuse", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
			{"Ambient", XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f) }, {"Specular", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }, {"FresnelR0", XMFLOAT3(0.1f, 0.1f, 0.1f) },
			{"Shininess", 0.98f },{"LightColor",  XMFLOAT3(1.0f, 1.0f, 1.0f)},{"FallOffStart", 0.0f }, {"Direction",  LightPool::SunDirection},
			{"PointPos", LightPool::AreaLightPosition }, {"FallOffEnd", 0.0f }
		};
		
		const auto& ptrMat = AddMaterial(matName, "Phong");
		ptrMat->BindCBuffer(renderer, tCBufferValue);
		ptrMat->BindTexture(diffuseName,renderer);
		ptrMat->BindTexture(normalName, renderer);
		ptrMat->BindTexture("ShadowMapTex", renderer);
#if SSAO
		ptrMat->BindTexture("SsaoTex", renderer);
#endif
		ptrMat->BindSampler(0);
		ptrMat->BindSampler(1);

		const auto& ptrGBufferMat = AddMaterial(gbufferMatName, "GBuffer");
		ptrGBufferMat->BindCBuffer(renderer, tCBufferValue);
		ptrGBufferMat->BindTexture(diffuseName, renderer);
		ptrGBufferMat->BindTexture(normalName, renderer);
		ptrGBufferMat->BindTexture("ShadowMapTex", renderer);
		ptrGBufferMat->BindSampler(0);
		ptrGBufferMat->BindSampler(1);

		return { ptrGBufferMat, ptrMat };
	}


	const std::shared_ptr<Material>& MaterialPool::GetMaterial(const std::string& InName)
	{
		const std::shared_ptr<Material>& tPtrMat = MaterialData[InName];
		if (tPtrMat == nullptr) throw "No Such Material";
		return tPtrMat;
	}

	void MaterialPool::UpdateMaterialData()
	{
		if (DataIsDirty)
		{
			const auto& material = GetMaterial("SSR");
			material->UpdateConstData("OpenIndirectLight", &CacheOpenIndirectLight, 1);
			DataIsDirty = false;
		}
	}
}
