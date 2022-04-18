#pragma once
#include <d3d11.h>
#include <Utility/Direct3D/Headers/D3D.h>
#include "Core/Entity System/Component.h"
#include <DirectXMath.h>
class LineMesh : public Component
{
public:

	LineMesh();
	~LineMesh();

	void setMatricies(DirectX::XMFLOAT4X4* view, DirectX::XMFLOAT4X4* projection);

	void createInstancedGrid(int width, int height, int depth);
	void setInstanceSize(int size) { instanceCount = size; };

	void Render();
	void Update(float deltaTime);

private:

	struct Vertex
	{
		Vertex() { Pos = DirectX::XMFLOAT3(0, 0, 0); Normal = DirectX::XMFLOAT3(0, 0, 0); TexCoord = DirectX::XMFLOAT2(0, 0); Color = DirectX::XMFLOAT2(0, 0); };
		Vertex(DirectX::XMFLOAT3 pos) { Pos = pos; Normal = DirectX::XMFLOAT3(0, 0, 0); TexCoord = DirectX::XMFLOAT2(0, 0); Color = DirectX::XMFLOAT2(0, 0); };
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT2 TexCoord;
		DirectX::XMFLOAT2 Color;
	};

	struct Instance
	{
		DirectX::XMFLOAT3 position;
	};

	struct InstanceData
	{
		DirectX::XMFLOAT3 gridPos;
	};

	struct MatrixBuffer
	{
		MatrixBuffer() : view(), projection() {};
		DirectX::XMFLOAT4X4* view;
		DirectX::XMFLOAT4X4* projection;
	};

	struct GridBuffer
	{
		DirectX::XMFLOAT3 gridDimensions;
		float pad;
	};

	struct MatrixConstantBuffer
	{
		DirectX::XMMATRIX mWorld;
		DirectX::XMMATRIX mView;
		DirectX::XMMATRIX mProjection;
	};

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11InputLayout* vertexLayout = nullptr;

	ID3D11PixelShader* pixelShader = nullptr;

	ID3D11Buffer* constantBuffer = nullptr;
	MatrixBuffer matrixBuffer;

	ID3D11Buffer* gridConstantBuffer = nullptr;
	GridBuffer gridBuffer;

	ID3D11BlendState* blendState = nullptr;

	ID3D11Buffer* instanceBuffer = nullptr;
	ID3D11Buffer* instanceDataBuffer = nullptr;
	int instanceCount = 50*50*50;

	int width, height, depth;

	D3D* direct3D = nullptr;
};

