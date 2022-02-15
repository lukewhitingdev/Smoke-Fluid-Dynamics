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
	DirectX::XMFLOAT4X4 getViewMatrix() { return viewMatrix; };
	DirectX::XMFLOAT4X4 getProjectionMatrix() { return projectionMatrix; };

	void Update(float deltaTime);
	void Render() {};

private:
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;

	Transform* transform;
	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 at;
	DirectX::XMFLOAT3 up;
};

