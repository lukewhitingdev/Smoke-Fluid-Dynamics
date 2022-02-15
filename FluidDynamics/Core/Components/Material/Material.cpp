#include "Material.h"
#include <Utility/Shader/ShaderUtility.h>

Material::Material()
{
	direct3D = D3D::getInstance();

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	if (FAILED(ShaderUtility::CompileShaderFromFile(L"Resources\\shader.fx", "PS", "ps_4_0", &pPSBlob)))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return;
	}

	// Create the pixel shader
	if (FAILED(direct3D->device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pixelShader)))
		throw;

	pPSBlob->Release();

	// load and setup textures
	if (FAILED(CreateDDSTextureFromFile(direct3D->device, L"Resources\\stone.dds", nullptr, &textureResourceView)))
		throw;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if(FAILED(direct3D->device->CreateSamplerState(&sampDesc, &sampler)))
		throw;

	materialCBProxy.Diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	materialCBProxy.Specular = DirectX::XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f);
	materialCBProxy.SpecularPower = 32.0f;
	materialCBProxy.UseTexture = true;

	// Create the material constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MaterialConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	if (FAILED(direct3D->device->CreateBuffer(&bd, nullptr, &materialConstantBuffer)))
		throw;

	this->setType(ComponentTypes::Material);
}

Material::~Material()
{
}

void Material::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);
	direct3D->immediateContext->UpdateSubresource(materialConstantBuffer, 0, nullptr, &materialCBProxy, 0, 0);
}

void Material::Render()
{
	direct3D->immediateContext->PSSetShader(pixelShader, nullptr, 0);
	direct3D->immediateContext->PSSetConstantBuffers(1, 1, &materialConstantBuffer);

	direct3D->immediateContext->PSSetShaderResources(0, 1, &textureResourceView);
	direct3D->immediateContext->PSSetSamplers(0, 1, &sampler);
}
