#include "CFDGrid.h"
#include <Dependencies\Physics\solver.c>
using namespace CFD;

CFDGrid::CFDGrid()
{
}

CFDGrid::~CFDGrid()
{
}

void CFDGrid::Start()
{
	D3D* direct3D = D3D::getInstance();

	D3D11_TEXTURE3D_DESC texDesc = {};
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE3D_DESC));
	texDesc.Width = N;
	texDesc.Height = N;
	texDesc.Depth = N;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32_FLOAT;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(direct3D->device->CreateTexture3D(&texDesc, nullptr, &voxelTex)))
		throw;

	D3D11_SHADER_RESOURCE_VIEW_DESC texViewDesc = {};
	ZeroMemory(&texViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	texViewDesc.Format = texDesc.Format;
	texViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	texViewDesc.Texture2D.MipLevels = texDesc.MipLevels;
	texViewDesc.Texture2D.MostDetailedMip = 0;

	direct3D->device->CreateShaderResourceView(voxelTex, &texViewDesc, &voxelView);

	// Create Sampler
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	direct3D->device->CreateSamplerState(&sampDesc, &sampler);

}

void CFDGrid::Update(float deltaTime)
{
	resetValuesForCurrentFrame();
	velocityStep(deltaTime);
	densityStep(deltaTime);

	printGrid();
}

void CFDGrid::Render()
{
	D3D* direct3D = D3D::getInstance();

	direct3D->immediateContext->UpdateSubresource(voxelTex, 0, nullptr, voxels->density.getCurrentArray(), sizeof(float) * N, 1);
	direct3D->immediateContext->PSSetSamplers(0, 1, &sampler);
	direct3D->immediateContext->PSSetShaderResources(0, 1, &voxelView);
}

void CFD::CFDGrid::resetValuesForCurrentFrame()
{
	for(int x = 0; x < N; ++x)
	{
		for (int y = 0; y < N; ++y)
		{
			for (int z = 0; z < N; ++z)
			{
				voxels->density.setCurrentValue(Vector3(x, y, z), 0);
				voxels->velocityY.setCurrentValue(Vector3(x, y, z), 0);
				voxels->velocityX.setCurrentValue(Vector3(x, y, z), 0);
			}
		}
	}
}

void CFD::CFDGrid::densityStep(float deltaTime)
{
	dens_step(N, voxels->density.getCurrentArray(), voxels->density.getPreviousArray(), 
				 voxels->velocityX.getCurrentArray(), voxels->velocityY.getCurrentArray(),
				 diffusionRate, deltaTime);
}

void CFD::CFDGrid::velocityStep(float deltaTime)
{
	vel_step(N, voxels->velocityX.getCurrentArray(), voxels->velocityY.getCurrentArray(),
				voxels->velocityX.getPreviousArray(), voxels->velocityY.getPreviousArray(), 
				viscocity, deltaTime);
}
