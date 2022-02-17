#include "LineMesh.h"
#include "Utility/Shader/ShaderUtility.h"
#include <Core/Entities/GameObject.h>

LineMesh::LineMesh()
{
	direct3D = D3D::getInstance();
	this->setType(ComponentTypes::LineMesh);

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
	if (FAILED(ShaderUtility::CompileShaderFromFile(L"Resources\\LineMeshShader.fx", "VSMain", "vs_4_0", &pVSBlob)))
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
	if (FAILED(ShaderUtility::CompileShaderFromFile(L"Resources\\LineMeshShader.fx", "PSMain", "ps_4_0", &pPSBlob)))
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

	// Create vertex buffer to specification
	Vertex vertices[19];

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

	bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * 18;
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
	bd.ByteWidth = sizeof(WORD) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	if (FAILED(direct3D->device->CreateBuffer(&bd, &InitData, &indexBuffer)))
		throw;
}

LineMesh::~LineMesh()
{
}

void LineMesh::setMatricies(DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 projection)
{
	matrixBuffer.mView = view;
	matrixBuffer.mProjection = projection;
}

void LineMesh::Update(float deltaTime)
{
	MatrixConstantBuffer cb;
	cb.mWorld = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(static_cast<GameObject*>(getParent())->getTransform()->getWorld()));
	cb.mView = DirectX::XMMatrixTranspose(XMLoadFloat4x4(&matrixBuffer.mView));
	cb.mProjection = DirectX::XMMatrixTranspose(XMLoadFloat4x4(&matrixBuffer.mProjection));

	direct3D->immediateContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);
}

void LineMesh::Render()
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

