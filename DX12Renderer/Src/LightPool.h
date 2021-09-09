#pragma once
#include "ConstValue.h"
#include <vector>
namespace rdr
{
	class ConstBufferPool;

	struct Light
	{
		DirectX::XMFLOAT3 color = { 1, 1, 1 };
		float fallOffStart = 100.0f;
		DirectX::XMFLOAT4 direction = { 0.0f, -0.8f, 0.3f, 0 };
		DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
		float fallOffEnd = 400.0f;
	};

	class LightPool
	{
	public:
		LightPool()
		{
			AddPointLight({ -1190, 630, 490 }, Red);
			AddPointLight({ 1190, 630, 490 }, Green);
			AddPointLight({ -1190, 630, -490 }, Purple);
			AddPointLight({ 1190, 630, -490 }, Blue);
			AddPointLight({ -1190, 200, 490 });
			AddPointLight({ 1190, 200, 490 });
			AddPointLight({ -1190, 200, -490 });
			AddPointLight({ 1190, 200, -490 });
#ifdef POINTSHADOW
			AddPointLight(MainPointPosition, White, 100, 2000);
#endif
			AddDirectionalLight(SunDirection);
		}
		LightPool(const std::vector<Light>& pointLights, const std::vector<Light>& directionalLighs)
			: pointLightVec(pointLights), directionalLightVec(directionalLighs) { }
		~LightPool() = default;

	public:
		void AddPointLight(const Light& light)
		{
			pointLightVec.emplace_back(light);
		}
		void AddPointLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color = White, float fallOffStart = 100.0f, float fallOffEnd = 400.0f)
		{
			Light temp;
			temp.position = position;
			temp.color = color;
			temp.fallOffStart = fallOffStart;
			temp.fallOffEnd = fallOffEnd;
			pointLightVec.emplace_back(temp);
		}
		void AddDirectionalLight(const Light& light)
		{
			directionalLightVec.emplace_back(light);
		}
		void AddDirectionalLight(DirectX::XMFLOAT4 direction, DirectX::XMFLOAT3 color = White)
		{
			Light temp;
			temp.direction = direction;
			temp.color = color;
			directionalLightVec.emplace_back(temp);
		}
		size_t GetPointNum() const { return pointLightVec.size(); }
		size_t GetDirectLightNum() const { return directionalLightVec.size(); }
		const Light& GetPointLight(size_t index) const { return pointLightVec[index]; }
		const Light& GetDirectionalLight(size_t index) const { return directionalLightVec[index]; }

	public:
		static constexpr DirectX::XMFLOAT4 SunDirection = { 0.0f, -0.8f, 0.3f, 0 };		//平行光方向
		static constexpr DirectX::XMFLOAT3 MainPointPosition = { 0, 600, 0 };		//主点光源位置
		static constexpr DirectX::XMFLOAT3 Red = { 1, 0, 0 };
		static constexpr DirectX::XMFLOAT3 Green = { 0, 1, 0 };
		static constexpr DirectX::XMFLOAT3 Blue = { 0, 0, 1 };
		static constexpr DirectX::XMFLOAT3 Purple = { 0.5f, 0, 0.5f };
		static constexpr DirectX::XMFLOAT3 Yellow = { 1, 1, 0 };
		static constexpr DirectX::XMFLOAT3 White = { 1, 1, 1 };

	private:
		std::vector<Light> pointLightVec;
		std::vector<Light> directionalLightVec;
	};
}