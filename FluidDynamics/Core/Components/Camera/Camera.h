#pragma once

#include "Core/Components/Transform/Transform.h"
#include <DirectXMath.h>
class Camera : public Component
{
public:
	Camera();
	~Camera();

	void setTransfrom(Transform* trans);
	void updateProjection(int width, int height);
	DirectX::XMFLOAT4X4* getViewMatrix() { return &viewMatrix; };
	DirectX::XMFLOAT4X4* getProjectionMatrix() { return &projectionMatrix; };

	void Update(float deltaTime);
	void Render() {};

	void setYaw(float value) { yaw = value; };
	void setPitch(float value) { pitch = value; };

private:

	float rightMag = 0;
	float leftMag = 0;
	float forwardMag = 0;
	float backMag = 0;

	float pitch = 0;
	float yaw = 0;

	int initialSpeed = 10;
	int speed;
	int boost = 5;

	DirectX::XMFLOAT4X4 rotation;

	DirectX::XMFLOAT3 defaultForward;
	DirectX::XMFLOAT3 defaultRight;
	DirectX::XMFLOAT3 defaultUp;

	DirectX::XMFLOAT3 forward;
	DirectX::XMFLOAT3 right;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 target;

	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;

	Transform* transform;
	DirectX::XMFLOAT3* position;
};

