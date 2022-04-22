#include "LineMesh.h"
#include "Utility/Shader/ShaderUtility.h"
#include <Core/Entities/GameObject.h>

LineMesh::LineMesh() : depth(), gridBuffer(), height(), selectedMesh(), width()
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

	// Create the constant buffer
	bd = {};
	bd.CPUAccessFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(GridBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	if (FAILED(direct3D->device->CreateBuffer(&bd, nullptr, &gridConstantBuffer)))
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

	D3D11_BLEND_DESC blendStateDesc{};
	blendStateDesc.AlphaToCoverageEnable = FALSE;
	blendStateDesc.IndependentBlendEnable = FALSE;
	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(direct3D->device->CreateBlendState(&blendStateDesc, &blendState)))
		throw;

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// Instance Pos.
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		// Instance Grid Pos.
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
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
	bd.ByteWidth = sizeof(WORD) * 19;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	if (FAILED(direct3D->device->CreateBuffer(&bd, &InitData, &indexBuffer)))
		throw;
}

LineMesh::~LineMesh()
{
}

void LineMesh::setMatricies(DirectX::XMFLOAT4X4* view, DirectX::XMFLOAT4X4* projection)
{
	matrixBuffer.view = view;
	matrixBuffer.projection = projection;
}

void LineMesh::createInstancedGrid(int w, int h, int d)
{
	width = w;
	height = h;
	depth = d;


	instanceCount = width * height * depth;
	Instance* instances;
	instances = new Instance[instanceCount];

	// TODO: Move this into the grid part or another part of the program.
	// TODO: Make it so that we can define the space in which we are simulating and the particle size and we will sub-divide the space into particle sized divisions.
	// Something like Space.xyz / Particle.xyz is the number of particles && the scale of the particles?.

	int index = 0;
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int z = 0; z < depth; z++)
			{
				if(index < instanceCount)
				{
					instances[index].position = DirectX::XMFLOAT3(float(x - (width / 2)), float(y - (height / 2)), float(z - (depth / 2)));
					index++;
				}
			}
		}
	}

	InstanceData* instanceData;
	instanceData = new InstanceData[instanceCount];

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int z = 0; z < depth; z++)
			{
				instanceData[width * height * z + height * y + x].gridPos = DirectX::XMFLOAT3(float(x),float(y),float(z));
			}
		}
	}

	// Setup instance buffer.
	D3D11_BUFFER_DESC bd = {};
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Instance) * instanceCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA instData = {};
	ZeroMemory(&instData, sizeof(D3D11_SUBRESOURCE_DATA));
	instData.pSysMem = instances;

	if (FAILED(direct3D->device->CreateBuffer(&bd, &instData, &instanceBuffer)))
		throw;

	// Setup instance data buffer.
	bd = {};
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(InstanceData) * instanceCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	instData = {};
	ZeroMemory(&instData, sizeof(D3D11_SUBRESOURCE_DATA));
	instData.pSysMem = instanceData;

	if (FAILED(direct3D->device->CreateBuffer(&bd, &instData, &instanceDataBuffer)))
		throw;

	delete[] instanceData;
	delete[] instances;
}

void LineMesh::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	MatrixConstantBuffer cb;
	cb.mWorld = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(static_cast<GameObject*>(getParent())->getTransform()->getWorld()));
	cb.mView = DirectX::XMMatrixTranspose(XMLoadFloat4x4(matrixBuffer.view));
	cb.mProjection = DirectX::XMMatrixTranspose(XMLoadFloat4x4(matrixBuffer.projection));

	direct3D->immediateContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);

	GridBuffer gb;
	DirectX::XMFLOAT3 dim = DirectX::XMFLOAT3(float(width), float(height), float(depth));
	gb.gridDimensions = DirectX::XMLoadFloat3(&dim);
	gb.selectedGridItem = DirectX::XMLoadFloat3(&selectedMesh);

	direct3D->immediateContext->UpdateSubresource(gridConstantBuffer, 0, nullptr, &gb, 0, 0);
}

void LineMesh::Render()
{
	direct3D->immediateContext->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);

	// Set the input layout
	direct3D->immediateContext->IASetInputLayout(vertexLayout);

	// Set vertex buffer
	UINT strides[3];
	UINT offsets[3];
	ID3D11Buffer* buffPointers[3];

	strides[0] = sizeof(Vertex);
	strides[1] = sizeof(Instance);
	strides[2] = sizeof(InstanceData);

	offsets[0] = 0;
	offsets[1] = 0;
	offsets[2] = 0;

	buffPointers[0] = vertexBuffer;
	buffPointers[1] = instanceBuffer;
	buffPointers[2] = instanceDataBuffer;

	direct3D->immediateContext->IASetVertexBuffers(0, 3, buffPointers, strides, offsets);

	// Set index buffer
	direct3D->immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	direct3D->immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	direct3D->immediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);
	direct3D->immediateContext->PSSetConstantBuffers(1, 1, &gridConstantBuffer);

	// Set the pixel shader.
	direct3D->immediateContext->VSSetShader(vertexShader, nullptr, 0);

	// Set the pixel shader.
	direct3D->immediateContext->PSSetShader(pixelShader, nullptr, 0);

	direct3D->immediateContext->DrawIndexedInstanced(18, instanceCount, 0, 0, 0);
}

