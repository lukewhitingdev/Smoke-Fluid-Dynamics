#pragma once
#include "Core/Entity System/Component.h"
#include "Utility/Direct3D/Headers/D3D.h"
#include <DirectXMath.h>
#include <vector>
#include <DirectXMath.h>
#include <Core/Components/Transform/Transform.h>
#include "Core/Components/LineMesh/LineMesh.h"
#include "Core/Entities/GameObject.h"
class Grid : public Component
{
public:
	Grid();
	~Grid();

	void setMatrices(DirectX::XMFLOAT4X4* world, DirectX::XMFLOAT4X4* view, DirectX::XMFLOAT4X4* projection);

	void GenerateGrid(const int Xmax, const int Ymax, const int Zmax);

	void Render();
	void Update(float deltaTime);

private:

	struct MatrixBuffer
	{
		DirectX::XMFLOAT4X4* mWorld;
		DirectX::XMFLOAT4X4* mView;
		DirectX::XMFLOAT4X4* mProjection;
	};

	struct MatrixConstantBuffer
	{
		DirectX::XMMATRIX mWorld;
		DirectX::XMMATRIX mView;
		DirectX::XMMATRIX mProjection;
	};

	std::vector<GameObject*> gridObjects;
	ID3D11Buffer* constantBuffer = nullptr;
	MatrixBuffer matrixBuffer;
	D3D* direct3D = nullptr;
};
