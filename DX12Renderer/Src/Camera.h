#pragma once
#include <DirectXMath.h>
using namespace DirectX;

namespace rdr
{
	//TODO:这里可以加上重置位置的功能，例如按下F键快速移动到中心位置
	class Camera
	{
	public:
		Camera();
		Camera(const Camera&) = delete;
		Camera& operator= (const Camera&) = delete;
		~Camera() = default;

	public:
		void Update();

	public:
		void MoveForward(float distance);
		void MoveRight(float distance);
		void MoveUp(float distance);
		void RotateY(float angle);
		void RotateX(float angle);

	public:
		XMMATRIX GetViewMatrix() const { return XMLoadFloat4x4(&viewMatrix); }
		XMMATRIX GetProjMatrix() const { return XMLoadFloat4x4(&projMatrix); }
		XMFLOAT3 GetCameraPosition() const { return cameraPosition; }
		float GetNearZ() const { return nearZ; }
		float GetFarZ() const { return farZ; }
		float GetMoveSpeed() const { return moveSpeed; }
		void SetMoveSpeed(float speed) { moveSpeed = speed; }

	private:
		XMFLOAT3 cameraPosition;
		XMFLOAT3 rightDir;
		XMFLOAT3 upDir;
		XMFLOAT3 forwardDir;

	private:
		XMFLOAT4X4 viewMatrix;
		XMFLOAT4X4 projMatrix;

	private:
		float fovAngleY;			//垂直视场角
		float aspect;			//纵横比 = 宽度 / 高度
		float nearZ;
		float farZ;

	private:
		bool needUpdate;
		float moveSpeed;
	};
}
