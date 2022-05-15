#pragma once

#include <Windows.h>
#include <memory>
#include <functional>
#include <DirectXMath.h>
using namespace DirectX;
namespace rdr
{
	class Camera;
	class GameTimer;
	class Renderer;

	class Input
	{
	public:
		Input(const std::function<void(float dx, float dy)>& ratateFunc);
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;
		~Input() = default;

	public:
		int GetLastPosX() { return  lastPostionX; }
		int GetLastPosY() { return lastPostionY; }

		void OnLeftMouseDown(int x, int y);

		void OnRightMouseDown(int x, int y)
		{
			lastPostionX = x;
			lastPostionY = y;
			isRightButtonDown = true;
		}

		void OnRightMouseUp()
		{
			isRightButtonDown = false;
		}

		void OnMouseMove(int x, int y)
		{
			//当按下右键时鼠标的移动才生效
			if (isRightButtonDown == true)
			{
				float dx = XMConvertToRadians(0.2f * static_cast<float>(x - lastPostionX));
				float dy = XMConvertToRadians(0.2f * static_cast<float>(y - lastPostionY));
				rotateCameraFunc(dx, dy);
				lastPostionX = x;
				lastPostionY = y;
			}
		}
		void Update(const GameTimer& gameTimer, Camera& camera, const Renderer& renderer);

	private:
		int lastPostionX;
		int lastPostionY;
		bool isRightButtonDown;
		float angle;

	private:
		std::function<void(float dx, float dy)> rotateCameraFunc;
	};
}