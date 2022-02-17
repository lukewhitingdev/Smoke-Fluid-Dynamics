#include "Camera.h"
#include "Core/Entities/GameObject.h"

Camera::Camera() : projectionMatrix(XMFLOAT4X4()), viewMatrix(XMFLOAT4X4()), transform(nullptr), eye(XMFLOAT3()), at(XMFLOAT3()), up(XMFLOAT3())
{
	// Initialize the view matrix
	eye = DirectX::XMFLOAT3(0, 0, -3);
	at = DirectX::XMFLOAT3(0, 0, 0);
	up = DirectX::XMFLOAT3(0, 1, 0);

	XMFLOAT4 Eye = XMFLOAT4(eye.x, eye.y, eye.z, 1);
	XMFLOAT4 At = XMFLOAT4(at.x, at.y, at.z, 1);
	XMFLOAT4 Up = XMFLOAT4(up.x, up.y, up.z, 1);

	XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixLookAtLH(XMLoadFloat4(&Eye), XMLoadFloat4(&At), XMLoadFloat4(&Up)));

	this->setType(ComponentTypes::Camera);
	this->setRenderable(false);
}

Camera::~Camera()
{
}

void Camera::setTransfrom(Transform* trans)
{
	transform = trans;
}

void Camera::updateProjection(int width, int height)
{
	//Only need to change this if we are resizing the window.
	XMStoreFloat4x4(&projectionMatrix,XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, width / (FLOAT)height, 0.01f, 100.0f));
}

void Camera::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	eye = transform->getPosition();

	XMFLOAT4 Eye = XMFLOAT4(eye.x, eye.y, eye.z, 1);
	XMFLOAT4 At = XMFLOAT4(at.x, at.y, at.z, 1);
	XMFLOAT4 Up = XMFLOAT4(up.x, up.y, up.z, 1);
	XMStoreFloat4x4(&viewMatrix,XMMatrixLookAtLH(XMLoadFloat4(&Eye), XMLoadFloat4(&At), XMLoadFloat4(&Up)));
}
