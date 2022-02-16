#pragma once
#include "Core/Entity System/Component.h"
#include "Utility/Direct3D/Headers/D3D.h"
#include <DirectXMath.h>
#include <vector>
#include <DirectXMath.h>
class Grid : public Component
{
public:
	struct Vertex
	{
		Vertex(DirectX::XMFLOAT3 pos) { Pos = pos; };
		DirectX::XMFLOAT3 Pos;
	};

	struct MatrixBuffer
	{
		DirectX::XMMATRIX mWorld;
		DirectX::XMMATRIX mView;
		DirectX::XMMATRIX mProjection;
	};

	Grid();
	~Grid();

	template<typename T>
	void GenerateGrid(const int Xmax, const int Ymax, const int Zmax) 
	{
		//const int vertexBufferSize = Xmax * Ymax * Zmax;
		const int vertexBufferSize = 4;

		// Create vertex buffer to specification
		std::vector<Vertex> vertices;

		vertices.push_back(Vertex(DirectX::XMFLOAT3(0, 0, 0)));
		vertices.push_back(Vertex(DirectX::XMFLOAT3(1, 0, 0)));
		vertices.push_back(Vertex(DirectX::XMFLOAT3(1, -1, 0)));
		vertices.push_back(Vertex(DirectX::XMFLOAT3(0, -1, 0)));

		//for(int i = 0; i < vertexBufferSize; i++)
		//{
		//	for (int x = 0; x < Xmax; x++)
		//	{
		//		for (int y = 0; y < Ymax; y++)
		//		{
		//			for (int z = 0; z < Zmax; z++)
		//			{
		//				vertices.push_back(Vertex(DirectX::XMFLOAT3(x, y, z)));
		//			}
		//		}
		//	}
		//}

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(Vertex) * vertexBufferSize;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = vertices.data();
		if (FAILED(direct3D->device->CreateBuffer(&bd, &InitData, &vertexBuffer)))
			throw;

		// Create index buffer
		WORD indices[] =
		{
			1,2,3,4
		};

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD) * 100;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		InitData.pSysMem = indices;
		if (FAILED(direct3D->device->CreateBuffer(&bd, &InitData, &indexBuffer)))
			throw;
	};

	void Render();
	void Update() {};

	ID3D11Buffer* getVertexBuffer() { return vertexBuffer; }
	ID3D11Buffer* getIndexBuffer() { return indexBuffer; }

private:

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11InputLayout* vertexLayout = nullptr;

	ID3D11Buffer* constantBuffer = nullptr;

	D3D* direct3D = nullptr;
};
