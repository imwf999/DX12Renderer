#pragma once
#include <memory>

namespace rdr
{
	class RenderTexture;
	class DepthTexture;
	class Renderer;
	class Material;
	class Mesh;

	class GBuffer
	{
	public:
		GBuffer(const Renderer& renderer);
		~GBuffer();

	public:
		void Draw();

	private:
		std::shared_ptr<RenderTexture> ptrScreenSpaceNormal;
		std::shared_ptr<RenderTexture> ptrScreenSpaceDiffuse;
		std::shared_ptr<RenderTexture> ptrWorldNormal;
		std::shared_ptr<RenderTexture> ptrDirectLight;
		std::shared_ptr<DepthTexture> ptrDepthTexture;
		std::shared_ptr<Mesh> ptrMesh;
		const Renderer* ptrRenderer;
	};
}
