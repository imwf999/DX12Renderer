#include "Camera.h"
#include "ConstValue.h"
#include "MathHelper.h"

namespace rdr
{
	Camera::Camera()
		: fovAngleY(0.25f * Math::Pi),
		aspect(global_WindowWidth / static_cast<float>(global_WindowHeight)),
		nearZ(50.0f),
		farZ(3900.0f),
		needUpdate(true),
		moveSpeed(100.0)
	{
		cameraPosition = { 0, 20, -100 };
		forwardDir = { 0, 0, 1 };
		rightDir = { 1, 0, 0 };
		upDir = { 0, 1, 0 };

		XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(XMLoadFloat3(&cameraPosition), XMVectorZero(), XMVectorSet(0, 1, 0, 0)));
		XMStoreFloat4x4(&projMatrix, XMMatrixPerspectiveFovLH(fovAngleY, aspect, nearZ, farZ));
	}

	void Camera::MoveForward(float distance)
	{
		XMVECTOR d = XMVectorReplicate(distance);
		XMVECTOR f = XMLoadFloat3(&forwardDir);
		XMVECTOR pos = XMLoadFloat3(&cameraPosition);
		XMStoreFloat3(&cameraPosition, XMVectorMultiplyAdd(d, f, pos));
		needUpdate = true;
	}

	void Camera::MoveRight(float distance)
	{
		XMVECTOR d = XMVectorReplicate(distance);
		XMVECTOR r = XMLoadFloat3(&rightDir);
		XMVECTOR pos = XMLoadFloat3(&cameraPosition);
		XMStoreFloat3(&cameraPosition, XMVectorMultiplyAdd(d, r, pos));
		needUpdate = true;
	}

	void Camera::MoveUp(float distance)
	{
		XMVECTOR d = XMVectorReplicate(distance);
		XMVECTOR u = XMLoadFloat3(&upDir);
		XMVECTOR pos = XMLoadFloat3(&cameraPosition);
		XMStoreFloat3(&cameraPosition, XMVectorMultiplyAdd(d, u, pos));
		needUpdate = true;
	}

	void Camera::RotateY(float angle)
	{
		XMMATRIX rotate = XMMatrixRotationY(angle);

		XMStoreFloat3(&rightDir, XMVector3TransformNormal(XMLoadFloat3(&rightDir), rotate));
		XMStoreFloat3(&upDir, XMVector3TransformNormal(XMLoadFloat3(&upDir), rotate));
		XMStoreFloat3(&forwardDir, XMVector3TransformNormal(XMLoadFloat3(&forwardDir), rotate));
		needUpdate = true;
	}

	void Camera::RotateX(float angle)
	{
		XMMATRIX rotate = XMMatrixRotationAxis(XMLoadFloat3(&rightDir), angle);

		XMStoreFloat3(&upDir, XMVector3TransformNormal(XMLoadFloat3(&upDir), rotate));
		XMStoreFloat3(&forwardDir, XMVector3TransformNormal(XMLoadFloat3(&forwardDir), rotate));

		needUpdate = true;
	}

	void Camera::Update()
	{
		if (needUpdate == true)
		{
			needUpdate = false;

			XMVECTOR right = XMLoadFloat3(&rightDir);
			XMVECTOR up = XMLoadFloat3(&upDir);
			XMVECTOR forward = XMLoadFloat3(&forwardDir);
			XMVECTOR position = XMLoadFloat3(&cameraPosition);

			forward = XMVector3Normalize(forward);
			up = XMVector3Normalize(XMVector3Cross(forward, right));
			right = XMVector3Cross(up, forward);

			float x = -XMVectorGetX(XMVector3Dot(position, right));
			float y = -XMVectorGetX(XMVector3Dot(position, up));
			float z = -XMVectorGetX(XMVector3Dot(position, forward));

			XMStoreFloat3(&rightDir, right);
			XMStoreFloat3(&upDir, up);
			XMStoreFloat3(&forwardDir, forward);

			viewMatrix(0, 0) = rightDir.x;
			viewMatrix(1, 0) = rightDir.y;
			viewMatrix(2, 0) = rightDir.z;
			viewMatrix(3, 0) = x;

			viewMatrix(0, 1) = upDir.x;
			viewMatrix(1, 1) = upDir.y;
			viewMatrix(2, 1) = upDir.z;
			viewMatrix(3, 1) = y;

			viewMatrix(0, 2) = forwardDir.x;
			viewMatrix(1, 2) = forwardDir.y;
			viewMatrix(2, 2) = forwardDir.z;
			viewMatrix(3, 2) = z;

			viewMatrix(0, 3) = 0.0f;
			viewMatrix(1, 3) = 0.0f;
			viewMatrix(2, 3) = 0.0f;
			viewMatrix(3, 3) = 1.0f;
		}
	}
}