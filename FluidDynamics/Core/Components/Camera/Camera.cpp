#include "Camera.h"
#include "Core/Entities/GameObject.h"
#include "Utility/Input System/InputSystem.h"
#include "Utility/Math/Math.h"

Camera::Camera() : speed(1), rotation(), forward(), right(), up(), target(), projectionMatrix(), viewMatrix(), transform(nullptr), position(nullptr)
{
	defaultForward = DirectX::XMFLOAT3(0, 0, 1);
	defaultRight = DirectX::XMFLOAT3(1, 0, 0);
	defaultUp = DirectX::XMFLOAT3(0, 1, 0);

	DirectX::XMFLOAT3 tempPosition = DirectX::XMFLOAT3(0, 0, 0);

	XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixLookAtLH(XMLoadFloat3(&tempPosition), XMLoadFloat3(&defaultForward), XMLoadFloat3(&defaultUp)));

	this->setType(ComponentTypes::Camera);
}

Camera::~Camera()
{
}

void Camera::setTransfrom(Transform* trans)
{
	transform = trans;
	delete position;
	position = transform->getPosition();
}

void Camera::updateProjection(int width, int height)
{
	//Only need to change this if we are resizing the window.
	XMStoreFloat4x4(&projectionMatrix,XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, width / (FLOAT)height, 0.01f, 1000.0f));
}

void Camera::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);


	// Mouse Look.
	InputSystem::Mouse currMousePos = InputSystem::getMousePosition();
	static InputSystem::Mouse prevMouse;
	if(prevMouse.x != currMousePos.x || prevMouse.y != currMousePos.y)
	{
		prevMouse = currMousePos;
		InputSystem::Mouse lastMousePos = InputSystem::getLastMousePosition();

		yaw += (lastMousePos.x - currMousePos.x) * deltaTime;
		pitch += (lastMousePos.y - currMousePos.y) * deltaTime;

		pitch = Math::clamp(pitch, -1.5f, 1.5f);
	}

	XMStoreFloat4x4(&rotation, XMMatrixRotationRollPitchYaw(pitch, yaw, 0));
	XMStoreFloat3(&target, XMVector3TransformCoord(XMLoadFloat3(&defaultForward), XMLoadFloat4x4(&rotation)));
	XMStoreFloat3(&right, XMVector3Normalize(XMVector3TransformCoord(XMLoadFloat3(&defaultRight), XMLoadFloat4x4(&rotation))));
	XMStoreFloat3(&forward, XMVector3Normalize(XMVector3TransformCoord(XMLoadFloat3(&defaultForward), XMLoadFloat4x4(&rotation))));
	XMStoreFloat3(&up, XMVector3Cross(XMLoadFloat3(&forward), XMLoadFloat3(&right)));

	DirectX::XMVECTOR posVec = XMLoadFloat3(position);

	// Movement.
	if (GetAsyncKeyState(0x57)) // W
	{
		posVec += (speed * deltaTime) * XMLoadFloat3(&forward);
	}

	if (GetAsyncKeyState(0x53)) // S
	{
		posVec -= (speed * deltaTime) * XMLoadFloat3(&forward);
	}

	if (GetAsyncKeyState(0x41)) // A
	{
		posVec -= (speed * deltaTime) * XMLoadFloat3(&right);
	}

	if (GetAsyncKeyState(0x44)) // D
	{
		posVec += (speed * deltaTime) * XMLoadFloat3(&right);
	}

	if(GetAsyncKeyState(VK_LSHIFT))
	{
		speed = initialSpeed * boost;
	}else
	{
		speed = initialSpeed;
	}

	XMStoreFloat4x4(&viewMatrix,XMMatrixLookToLH(posVec, XMLoadFloat3(&target), XMLoadFloat3(&up)));

	XMStoreFloat3(position, posVec);
}
