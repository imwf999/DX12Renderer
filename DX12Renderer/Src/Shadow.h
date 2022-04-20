#pragma once
#include <memory>
#include <vector>
#include <d3d12.h>
#include "ConstValue.h"

namespace rdr
{
	using namespace DirectX;

	class RenderTexture;
	class DepthTexture;
	class Renderer;
	class DescriptorPool;
	class Material;
	class Mesh;
	class Shader;
	class Camera;
	class FrameConstBuffer;

	class Shadow
	{
	public:
		Shadow(const Renderer& renderer);
		Shadow(const Shadow&) = delete;
		Shadow(Shadow&&) = delete;
		~Shadow();

	public:
		void DrawShadowMap();
		const Shader& GetShadowMapShader() const;

	public:
		static const XMMATRIX& ShadowViewMatrix() { return ViewMatrix; }
		static const XMMATRIX& ShadowProjMatrix() { return ProjMatrix; }
		static XMMATRIX ShadowMVPMatrix();

	private:
		D3D12_VIEWPORT ViewPort = { 0, 0, static_cast<float>(global_ShadowMapWidth), static_cast<float>(global_ShadowMapHeight), 0, 1.0f };
		D3D12_RECT Rect = { 0, 0,static_cast<LONG>(global_ShadowMapWidth), static_cast<LONG>(global_ShadowMapHeight) };
		std::shared_ptr<Material> ptrMaterial;
		std::shared_ptr<DepthTexture> ptrDepthTexture;
		std::shared_ptr<Camera> ptrShadowCamera;
		const Renderer* ptrRenderer;

	private:
		static XMMATRIX ViewMatrix;
		static XMMATRIX ProjMatrix;
	};
}
