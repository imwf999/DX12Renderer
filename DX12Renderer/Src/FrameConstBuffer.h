#pragma once
#include <memory>
#include <DirectXMath.h>
#include "MathHelper.h"

namespace rdr
{
	using namespace DirectX;
	class ConstantBuffer;
	class Camera;
	class Renderer;

	class FrameConstBuffer
	{
	public:
		FrameConstBuffer(const Renderer& renderer);
		~FrameConstBuffer();

	private:
		struct FrameCBufferData
		{
			XMFLOAT4X4 ViewMatrix = Math::Identity4x4();
			XMFLOAT4X4 ProjMatrix = Math::Identity4x4();
			XMFLOAT4X4 ViewProjMatrix = Math::Identity4x4();
			XMFLOAT4X4 ShadowViewProjMatrix = Math::Identity4x4();
			XMFLOAT4X4 ProjTextureMatrix = Math::Identity4x4();
			XMFLOAT4X4 InverseProjMatrix = Math::Identity4x4();
			XMFLOAT4X4 ShadowMVPMatrix = Math::Identity4x4();
			XMFLOAT4 EyePostion = { 0, 0, 0, 1 };
		};

	public:
		void Update();
		const ConstantBuffer& GetBuffer() const { return *ptrCBuffer; }

	private:
		FrameCBufferData data;
		std::unique_ptr<ConstantBuffer> ptrCBuffer;
		std::shared_ptr<Camera> ptrCamera;
	};
}
