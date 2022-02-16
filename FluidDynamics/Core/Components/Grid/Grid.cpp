#include "Grid.h"
#include "Utility/Shader/ShaderUtility.h"

Grid::Grid()
{
	direct3D = D3D::getInstance();
	this->setType(ComponentTypes::Grid);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.CPUAccessFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MatrixBuffer);
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

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

void Grid::Render()
{
	// Set vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	direct3D->immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Set index buffer
	direct3D->immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set index buffer
	direct3D->immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	direct3D->immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	direct3D->immediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	// Set the vertex buffer.
	direct3D->immediateContext->VSSetShader(vertexShader, nullptr, 0);
}
