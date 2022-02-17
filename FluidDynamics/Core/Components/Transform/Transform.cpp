#include "Transform.h"

Transform::Transform()
{
	DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixIdentity());;
	position = DirectX::XMFLOAT3(0, 0, 0);
	scale = DirectX::XMFLOAT3(1, 1, 1);
	rotation = DirectX::XMFLOAT3(0, 0, 0);

	this->setType(ComponentTypes::Transform);
	this->setRenderable(false);
}

Transform::~Transform()
{
}

void Transform::Update(float deltaTime)
{
	//rotation.y += deltaTime;
	//rotation.z += deltaTime / 2;

	DirectX::XMMATRIX mTranslate = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX mScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX mRotation = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	DirectX::XMMATRIX mWorld = mScale * mRotation * mTranslate;

	XMStoreFloat4x4(&world, mWorld);
}
