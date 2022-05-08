#include "TexturePool.h"
#include "CommonTexture.h"
#include "Renderer.h"
#include "DepthTexture.h"
#include "RendererHelper.h"
#include "RenderTexture.h"
#include <filesystem>

namespace rdr
{
	//TODO:这个读取纹理的方式还要改
	TexturePool::TexturePool(const Renderer& renderer)
	{
		using CommonTexPtr = std::shared_ptr<CommonTexture>;
		using DepthTexPtr = std::shared_ptr<DepthTexture>;
		using RenderTexPtr = std::shared_ptr<RenderTexture>;

		//读取文件夹下的所有diffuse和normal纹理
		std::string path = "Texture\\pbr";
		auto result = std::filesystem::directory_iterator(path);
		for (auto& entry : result)
		{
			std::wstring texturePath = entry.path();
			auto dotLoc = texturePath.find_last_of(L'.');
			if(texturePath.substr(dotLoc + 1, texturePath.size() - dotLoc - 1) != L"dds")
				continue;
			size_t begin = texturePath.find_last_of('\\');
			size_t end = texturePath.find_last_of('.');
			std::string name = WstrToStr(texturePath.substr(begin + 1, end - begin - 1));
			CommonTexPtr ptrTexture = std::make_shared<CommonTexture>(name);
			ptrTexture->CreateTextureFromFlie(texturePath, renderer, true);
			AddTexture(ptrTexture);
		}

		//加载默认纹理
		CommonTexPtr ptrDefaultDiffuse = std::make_shared<CommonTexture>("DefaultDiffuse");
		ptrDefaultDiffuse->CreateTextureFromFlie(L"Texture\\default\\default_diffuse.dds", renderer, true);
		CommonTexPtr ptrDefaultNormal = std::make_shared<CommonTexture>("DefaultNormal");
		ptrDefaultNormal->CreateTextureFromFlie(L"Texture\\default\\default_normal.dds", renderer, true);
		AddTexture(ptrDefaultDiffuse);
		AddTexture(ptrDefaultNormal);

		//用于天空盒的立方体纹理
		CommonTexPtr pSkybox = std::make_shared<CommonTexture>("SkyBox");
		pSkybox->CreateTextureFromFlie(L"Texture\\cubemap.dds", renderer, false);
		AddTexture(pSkybox);

		//用于最后一趟pass绘制场景所需的的深度纹理
		DepthTexPtr depthTex = std::make_shared<DepthTexture>("PhongDepthTex");
		depthTex->Create2D(renderer, DepthTexture::DefaultFormat, global_WindowWidth, global_WindowHeight);
		AddTexture(depthTex);

		//用于阴影贴图的深度纹理
		DepthTexPtr shadowMap = std::make_shared<DepthTexture>("ShadowMapTex");
		shadowMap->Create2D(renderer, DepthTexture::ShadowFormat, global_ShadowMapWidth, global_ShadowMapHeight);
		AddTexture(shadowMap);

		//用于存储G-Buffer中的像素法线， 摄像机空间下
		RenderTexPtr normalTex = std::make_shared<RenderTexture>("ScreenNormal", renderer, RenderTexture::ScreenNormalMapFormat);
		AddTexture(normalTex);

		//存储G-Buffer的像素漫反射反射率
		RenderTexPtr diffuseTex = std::make_shared<RenderTexture>("GBufferDiffuse", renderer, RenderTexture::ScreenNormalMapFormat);
		AddTexture(diffuseTex);

		//存储G-Buffer中像素法线，世界空间下
		RenderTexPtr worldNormalTex = std::make_shared<RenderTexture>("ScreenWorldNormal", renderer, RenderTexture::ScreenNormalMapFormat);
		AddTexture(worldNormalTex);

		//存储G-Buffer的直接光照
		RenderTexPtr directLightTex = std::make_shared<RenderTexture>("GBufferDirectLight", renderer, RenderTexture::ScreenNormalMapFormat);
		AddTexture(directLightTex);

		//用于存储G-Buffer中的像素深度
		DepthTexPtr ssaoDepthTex = std::make_shared<DepthTexture>("SsaoDepthTex");		//TODO:需要改名
		ssaoDepthTex->Create2D(renderer, DepthTexture::ShadowFormat, global_WindowWidth, global_WindowHeight);
		AddTexture(ssaoDepthTex);

		//用于SSAO图，用完后这张纹理会重复利用，和下面的SsaoBlur纹理一起使用。
		RenderTexPtr ssaoTex = std::make_shared<RenderTexture>("SsaoTex", renderer, RenderTexture::SsaoMapFormat, global_SSAOMapWidth, global_SSAOMapHeight);
		AddTexture(ssaoTex);

		//存储模糊后的SSAO纹理，模糊过程有三次循环，共6趟渲染，上一个作为输入，这个作为输出，然后下一趟又把这个作为输入，上一张纹理作为输出，以此类推
		RenderTexPtr ssaoBlurTex = std::make_shared<RenderTexture>("SsaoBlur", renderer, RenderTexture::SsaoMapFormat, global_SSAOMapWidth, global_SSAOMapHeight);
		AddTexture(ssaoBlurTex);

		//用于SSAO中的随机向量纹理
		CommonTexPtr randomVecTex = std::make_shared<CommonTexture>("RandomVecTex");
		randomVecTex->CreateRandomTexture(renderer, CommonTexture::RandomVectorFormat, global_SSAORandomVecMapWidth, global_SSAORandomVecMapHeight, true);
		AddTexture(randomVecTex);

	}

	TexturePool::~TexturePool() = default;
}
