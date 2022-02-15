#pragma once
#include "Core/Entity System/Component.h"
#include "Utility/Direct3D/Headers/D3D.h"
#include <DirectXMath.h>
class Mesh : public Component
{
public:
	struct SimpleVertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT2 TexCoord;
	};

	Mesh();
	~Mesh();

	void Render();
	void Update() {};

	ID3D11Buffer* getVertexBuffer() { return vertexBuffer; }
	ID3D11Buffer* getIndexBuffer() { return indexBuffer; }

private:

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11InputLayout* vertexLayout = nullptr;

	D3D* direct3D = nullptr;
};

