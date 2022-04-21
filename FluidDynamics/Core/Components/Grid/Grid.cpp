#include "Grid.h"
#include "Utility/Shader/ShaderUtility.h"
#include <Core/Entities/GameObject.h>

Grid::Grid() : matrixBuffer()
{
	direct3D = D3D::getInstance();
	this->setType(ComponentTypes::Grid);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.CPUAccessFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MatrixConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	if (FAILED(direct3D->device->CreateBuffer(&bd, nullptr, &constantBuffer)))
		throw;
}

Grid::~Grid()
{
}

void Grid::setMatrices(DirectX::XMFLOAT4X4* world, DirectX::XMFLOAT4X4* view, DirectX::XMFLOAT4X4* projection)
{
	matrixBuffer.mWorld = world;
	matrixBuffer.mView = view;
	matrixBuffer.mProjection = projection;
}

void Grid::GenerateGrid(const int Xmax, const int Ymax, const int Zmax)
{
	width = Xmax;
	height = Ymax;
	depth = Zmax;

	const unsigned long vertexBufferSize = width * height * depth;

	lineMeshInstancer = new GameObject();
	lineMeshComponent = lineMeshInstancer->addComponent<LineMesh>();
	lineMeshComponent->setMatricies(matrixBuffer.mView, matrixBuffer.mProjection);
	lineMeshComponent->createInstancedGrid(width, height, depth);
	lineMeshInstancer->removeMesh();
	lineMeshInstancer->removeMaterial();
}

void Grid::Render()
{
	if(lineMeshInstancer)
		lineMeshInstancer->draw();
}

void Grid::Update(float deltaTime)
{
	if (lineMeshInstancer)
		lineMeshInstancer->update(deltaTime);

	MatrixConstantBuffer cb;
	cb.mWorld = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(static_cast<GameObject*>(getParent())->getTransform()->getWorld()));
	cb.mView = DirectX::XMMatrixTranspose(XMLoadFloat4x4(matrixBuffer.mView));
	cb.mProjection = DirectX::XMMatrixTranspose(XMLoadFloat4x4(matrixBuffer.mProjection));

	direct3D->immediateContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);
}
