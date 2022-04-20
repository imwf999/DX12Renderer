#include "Input.h"
#include "Camera.h"
#include "GameTimer.h"
#include "ConstValue.h"

namespace rdr
{
	Input::Input(const std::function<void(float dx, float dy)>& ratateFunc)
		: angle(0), lastPostionX(0), lastPostionY(0), isRightButtonDown(false), rotateCameraFunc(ratateFunc)
	{

	}

	void Input::Update(const GameTimer& gameTimer, Camera& camera)
	{
		if (isRightButtonDown == true)
		{
			float deltaTime = gameTimer.DeltaTime();
			if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
				camera.SetMoveSpeed(global_cameraMoveHighSpeed);
			else
				camera.SetMoveSpeed(global_cameraMoveLowSpeed);
			if (GetAsyncKeyState('W') & 0x8000)
				camera.MoveForward(camera.GetMoveSpeed() * deltaTime);
			if (GetAsyncKeyState('S') & 0x8000)
				camera.MoveForward(-camera.GetMoveSpeed() * deltaTime);
			if (GetAsyncKeyState('A') & 0x8000)
				camera.MoveRight(-camera.GetMoveSpeed() * deltaTime);
			if (GetAsyncKeyState('D') & 0x8000)
				camera.MoveRight(camera.GetMoveSpeed() * deltaTime);
			if (GetAsyncKeyState('Q') & 0x8000)
				camera.MoveUp(-camera.GetMoveSpeed() * deltaTime);
			if (GetAsyncKeyState('E') & 0x8000)
				camera.MoveUp(camera.GetMoveSpeed() * deltaTime);
		}
	}
}