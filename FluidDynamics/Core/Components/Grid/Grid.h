#pragma once
#include "Core/Entity System/Component.h"
#include "Utility/Direct3D/Headers/D3D.h"
#include <DirectXMath.h>
#include <vector>
#include <DirectXMath.h>
#include <Core/Components/Transform/Transform.h>
class Grid : public Component
{
public:
	struct Vertex
	{
		Vertex() { Pos = DirectX::XMFLOAT3(0, 0, 0); Normal = DirectX::XMFLOAT3(0, 0, 0); TexCoord = DirectX::XMFLOAT2(0, 0); };
		Vertex(DirectX::XMFLOAT3 pos) { Pos = pos; Normal = DirectX::XMFLOAT3(0, 0, 0); TexCoord = DirectX::XMFLOAT2(0, 0); };
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT2 TexCoord;
	};

	struct MatrixBuffer
	{
		DirectX::XMFLOAT4X4 mWorld;
		DirectX::XMFLOAT4X4 mView;
		DirectX::XMFLOAT4X4 mProjection;
	};

	struct MatrixConstantBuffer
	{
		DirectX::XMMATRIX mWorld;
		DirectX::XMMATRIX mView;
		DirectX::XMMATRIX mProjection;
	};

	Grid();
	~Grid();

	void setMatrices(DirectX::XMFLOAT4X4 world, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 projection);

	template<typename T>
	void GenerateGrid(const int Xmax, const int Ymax, const int Zmax) 
	{
		const long long vertexBufferSize = Xmax * Ymax * Zmax;

		// Create vertex buffer to specification
		Vertex* vertices = new Vertex[vertexBufferSize];

		// Abstract this to another entity so we can move it in world space.
		vertices[0].Pos = DirectX::XMFLOAT3(0, 0, 0);
		vertices[1].Pos = DirectX::XMFLOAT3(1, 0, 0);		
		vertices[2].Pos = DirectX::XMFLOAT3(1, 1, 0);
		vertices[3].Pos = DirectX::XMFLOAT3(0, 1, 0);
		vertices[4].Pos = DirectX::XMFLOAT3(0, 0, 0);
		vertices[5].Pos = DirectX::XMFLOAT3(0, 0, -1);
		vertices[6].Pos = DirectX::XMFLOAT3(1, 0, -1);
		vertices[7].Pos = DirectX::XMFLOAT3(1, 0, 0);
		vertices[8].Pos = DirectX::XMFLOAT3(1, 1, 0);
		vertices[9].Pos = DirectX::XMFLOAT3(1, 1, -1);
		vertices[10].Pos = DirectX::XMFLOAT3(0, 1, -1);
		vertices[11].Pos = DirectX::XMFLOAT3(0, 1, 0);
		vertices[12].Pos = DirectX::XMFLOAT3(0, 1, -1);
		vertices[13].Pos = DirectX::XMFLOAT3(0, 0, -1);
		vertices[14].Pos = DirectX::XMFLOAT3(1, 0, -1);
		vertices[15].Pos = DirectX::XMFLOAT3(1, 1, -1);
		vertices[16].Pos = DirectX::XMFLOAT3(1, 1, 0);
		vertices[17].Pos = DirectX::XMFLOAT3(1, 0, 0);
		vertices[18].Pos = DirectX::XMFLOAT3(0, 0, 0);

		//for(int i = 0; i < vertexBufferSize; i++)
		//{
		//	for (int x = 0; x < Xmax; x++)
		//	{
		//		for (int y = 0; y < Ymax; y++)
		//		{
		//			for (int z = 0; z < Zmax; z++)
		//			{
		//				vertices[i].Pos = DirectX::XMFLOAT3(x, y, z);
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
		InitData.pSysMem = vertices;
		if (FAILED(direct3D->device->CreateBuffer(&bd, &InitData, &vertexBuffer)))
			throw;

		// Create index buffer
		WORD indices[] =
		{
			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18
		};

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD) * vertexBufferSize;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		InitData.pSysMem = indices;
		if (FAILED(direct3D->device->CreateBuffer(&bd, &InitData, &indexBuffer)))
			throw;
	};

	void Render();
	void Update(float deltaTime);

	ID3D11Buffer* getVertexBuffer() { return vertexBuffer; }
	ID3D11Buffer* getIndexBuffer() { return indexBuffer; }

private:

	Transform* transform = nullptr;

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11InputLayout* vertexLayout = nullptr;

	ID3D11PixelShader* pixelShader = nullptr;

	ID3D11Buffer* constantBuffer = nullptr;
	MatrixBuffer matrixBuffer;

	D3D* direct3D = nullptr;
};
