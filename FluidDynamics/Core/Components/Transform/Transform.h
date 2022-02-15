#pragma once
#include "Core/Entity System/Component.h"
#include <DirectXMath.h>
class Transform : public Component
{
public:
	Transform();
	~Transform();

	virtual void Update(float deltaTime);
	virtual void Render(float deltaTime) { UNREFERENCED_PARAMETER(deltaTime); };

	void setPosition(DirectX::XMFLOAT3 POSITION) { position = POSITION; };
	DirectX::XMFLOAT3 getPosition() { return position; }
	DirectX::XMFLOAT4X4* getWorld() { return &world; };

private:
	DirectX::XMFLOAT4X4	world;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotation;

	int mod = 1;
};

