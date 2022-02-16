#include "Grid.h"
#include "Utility/Shader/ShaderUtility.h"
#include <Core/Entities/GameObject.h>

Grid::Grid()
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

	ID3DBlob* pVSBlob = nullptr;
	if (FAILED(ShaderUtility::CompileShaderFromFile(L"Resources\\GridShader.fx", "VSMain", "vs_4_0", &pVSBlob)))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return;
	}

	// Create the vertex shader
	if (FAILED(direct3D->device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &vertexShader)))
	{
		pVSBlob->Release();
		throw;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	if (FAILED(ShaderUtility::CompileShaderFromFile(L"Resources\\GridShader.fx", "PSMain", "ps_4_0", &pPSBlob)))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return;
	}

	// Create the pixel shader
	if (FAILED(direct3D->device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pixelShader)))
		throw;

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	if (FAILED(direct3D->device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &vertexLayout)))
		throw;

	pVSBlob->Release();

	// Set the input layout
	direct3D->immediateContext->IASetInputLayout(vertexLayout);
}

Grid::~Grid()
{
}

void Grid::setMatrices(DirectX::XMFLOAT4X4 world, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 projection)
{
	
	matrixBuffer.mWorld = world;
	matrixBuffer.mView = view;
	matrixBuffer.mProjection = projection;
}

void Grid::Update(float deltaTime)
{
	Transform* transform = static_cast<GameObject*>(getParent())->getComponent<Transform>();

	MatrixConstantBuffer cb;
	cb.mWorld = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(transform->getWorld()));
	cb.mView = DirectX::XMMatrixTranspose(XMLoadFloat4x4(&matrixBuffer.mView));
	cb.mProjection = DirectX::XMMatrixTranspose(XMLoadFloat4x4(&matrixBuffer.mProjection));

	direct3D->immediateContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);
}

void Grid::Render()
{
	// Set vertex buffer
 	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	direct3D->immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Set index buffer
	direct3D->immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	direct3D->immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	direct3D->immediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	// Set the pixel shader.
	direct3D->immediateContext->VSSetShader(vertexShader, nullptr, 0);

	// Set the pixel shader.
	direct3D->immediateContext->PSSetShader(pixelShader, nullptr, 0);
}
