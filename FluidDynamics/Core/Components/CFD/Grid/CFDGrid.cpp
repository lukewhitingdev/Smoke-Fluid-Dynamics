#include "CFDGrid.h"
#include <Dependencies\Physics\solver.c>
#include <iostream>
#include <fstream>
using namespace CFD;

CFDGrid::CFDGrid() : N(0)
{
}

CFDGrid::~CFDGrid()
{
}

void CFDGrid::Start()
{
	simulating = true;

	D3D* direct3D = D3D::getInstance();

	D3D11_TEXTURE3D_DESC texDesc = {};
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE3D_DESC));
	texDesc.Width = N;
	texDesc.Height = N;
	texDesc.Depth = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32_FLOAT;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(direct3D->device->CreateTexture3D(&texDesc, nullptr, &voxelDensTex)))
		throw;

	D3D11_SHADER_RESOURCE_VIEW_DESC texViewDesc = {};
	ZeroMemory(&texViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	texViewDesc.Format = texDesc.Format;
	texViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	texViewDesc.Texture2D.MipLevels = texDesc.MipLevels;
	texViewDesc.Texture2D.MostDetailedMip = 0;

	direct3D->device->CreateShaderResourceView(voxelDensTex, &texViewDesc, &voxelDensView);

	texDesc = {};
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE3D_DESC));
	texDesc.Width = N;
	texDesc.Height = N;
	texDesc.Depth = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(direct3D->device->CreateTexture3D(&texDesc, nullptr, &voxelVeloTex)))
		throw;

	texViewDesc = {};
	ZeroMemory(&texViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	texViewDesc.Format = texDesc.Format;
	texViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	texViewDesc.Texture2D.MipLevels = texDesc.MipLevels;
	texViewDesc.Texture2D.MostDetailedMip = 0;

	if (FAILED(direct3D->device->CreateShaderResourceView(voxelVeloTex, &texViewDesc, &voxelVeloView)))
		throw;

	// Create Sampler
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	direct3D->device->CreateSamplerState(&sampDesc, &sampler);

}

static int frame;
void CFDGrid::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	if(simulating)
	{
		printf("Frame %d \n", frame);

		resetValuesForCurrentFrame();

		updateForces();

		addRandomVelocity();

		velocityStep(0.1f);
		densityStep(0.1f);

		//printGrid();

		frame++;
	}
}

void CFDGrid::Render()
{
	if (simulating)
	{
		D3D* direct3D = D3D::getInstance();

		// Move density data into proper texture format.
		int index = 0;
		for (int y = 0; y < N; ++y)
		{
			for (int x = 0; x < N; ++x)
			{
				densityTextureData[index] = voxels->density->getCurrentValue(Vector3(x, y, 0));

				float xVelo, yVelo;
				xVelo = voxels->velocityX->getCurrentValue(Vector3(x, y, 0));
				yVelo = voxels->velocityY->getCurrentValue(Vector3(x, y, 0));

				velocityTextureData[index] = Vector4(xVelo, yVelo, 0, 0);
				index++;
			}
		}

		direct3D->immediateContext->UpdateSubresource(voxelDensTex, 0, nullptr, densityTextureData, sizeof(float) * N, 1);

		// Seems to be issue with alignment or offset when setting the resource.
		direct3D->immediateContext->UpdateSubresource(voxelVeloTex, 0, nullptr, velocityTextureData, sizeof(Vector4) * N, 1);

		direct3D->immediateContext->PSSetSamplers(0, 1, &sampler);
		direct3D->immediateContext->PSSetShaderResources(0, 1, &voxelDensView);
		direct3D->immediateContext->PSSetShaderResources(1, 1, &voxelVeloView);
	}
}

void CFD::CFDGrid::addDensity(const Vector3& pos, const float val)
{
	queuedDensities.emplace_back(QueueItem<float>(pos, val));
}

void CFD::CFDGrid::addVelocity(const Vector3& pos, const Vector3& val)
{
	queuedVelocities.emplace_back(QueueItem<Vector3>(pos, val));
}

CFDVoxel CFD::CFDGrid::getVoxel(const Vector3& pos)
{
	CFDVoxel vox = CFDVoxel();
	if(simulating)
	{
		vox.position = pos;
		vox.density = voxels->density->getCurrentValue(pos);
		vox.velocity = Vector3(voxels->velocityX->getCurrentValue(pos),
			voxels->velocityY->getCurrentValue(pos),
			0.0f);
	}
	return vox;
}

void CFD::CFDGrid::resetValuesForCurrentFrame()
{
	for(int x = 0; x <= N; ++x)
	{
		for (int y = 0; y <= N; ++y)
		{
			for (int z = 0; z <= N; ++z)
			{
				voxels->density->setCurrentValue(Vector3(x, y, z), 0);
				voxels->velocityY->setCurrentValue(Vector3(x, y, z), 0);
				voxels->velocityX->setCurrentValue(Vector3(x, y, z), 0);
			}
		}
	}
}

void CFD::CFDGrid::addRandomVelocity()
{
	int randomX = Math::random(0, getGridWidth());
	int randomY = Math::random(0, getGridHeight());

	int randomValX = Math::random(-randomVelocityMinMax, randomVelocityMinMax);
	int randomValY = Math::random(-randomVelocityMinMax, randomVelocityMinMax);
	int randomValZ = Math::random(-randomVelocityMinMax, randomVelocityMinMax);

	addVelocity(Vector3(randomX, randomY, 0), Vector3(randomValX, randomValY, randomValZ));
}

void CFD::CFDGrid::updateForces()
{
	for(const auto& dens : queuedDensities)
	{
		voxels->density->setCurrentValue(dens.pos, voxels->density->getPreviousValue(dens.pos) + dens.value);
	}

	for (const auto& velo : queuedVelocities)
	{
		voxels->velocityX->setCurrentValue(velo.pos, voxels->velocityX->getPreviousValue(velo.pos) + velo.value.x);
		voxels->velocityY->setCurrentValue(velo.pos, voxels->velocityY->getPreviousValue(velo.pos) + velo.value.y);
	}
}

void CFD::CFDGrid::densityStep(float deltaTime)
{
	dens_step(N, voxels->density->getCurrentArray(), voxels->density->getPreviousArray(), 
				 voxels->velocityX->getCurrentArray(), voxels->velocityY->getCurrentArray(),
				 diffusionRate, deltaTime);
}

void CFD::CFDGrid::velocityStep(float deltaTime)
{
	vel_step(N, voxels->velocityX->getCurrentArray(), voxels->velocityY->getCurrentArray(),
				voxels->velocityX->getPreviousArray(), voxels->velocityY->getPreviousArray(), 
				viscocity, deltaTime);
}

void CFD::CFDGrid::setDebugVelocityValues()
{
	voxels->velocityX->setCurrentValue(Vector3(0, 1, 0), 01);
	voxels->velocityX->setCurrentValue(Vector3(0, 2, 0), 02);
	voxels->velocityX->setCurrentValue(Vector3(0, 3, 0), 03);
	voxels->velocityX->setCurrentValue(Vector3(0, 4, 0), 04);

	voxels->velocityX->setCurrentValue(Vector3(1, 0, 0), 10);
	voxels->velocityX->setCurrentValue(Vector3(1, 1, 0), 11);
	voxels->velocityX->setCurrentValue(Vector3(1, 2, 0), 12);
	voxels->velocityX->setCurrentValue(Vector3(1, 3, 0), 13);
	voxels->velocityX->setCurrentValue(Vector3(1, 4, 0), 14);

	voxels->velocityX->setCurrentValue(Vector3(2, 0, 0), 20);
	voxels->velocityX->setCurrentValue(Vector3(2, 1, 0), 21);
	voxels->velocityX->setCurrentValue(Vector3(2, 2, 0), 22);
	voxels->velocityX->setCurrentValue(Vector3(2, 3, 0), 23);
	voxels->velocityX->setCurrentValue(Vector3(2, 4, 0), 24);

	voxels->velocityX->setCurrentValue(Vector3(3, 0, 0), 30);
	voxels->velocityX->setCurrentValue(Vector3(3, 1, 0), 31);
	voxels->velocityX->setCurrentValue(Vector3(3, 2, 0), 32);
	voxels->velocityX->setCurrentValue(Vector3(3, 3, 0), 33);
	voxels->velocityX->setCurrentValue(Vector3(3, 4, 0), 34);

	voxels->velocityX->setCurrentValue(Vector3(4, 0, 0), 40);
	voxels->velocityX->setCurrentValue(Vector3(4, 1, 0), 41);
	voxels->velocityX->setCurrentValue(Vector3(4, 2, 0), 42);
	voxels->velocityX->setCurrentValue(Vector3(4, 3, 0), 43);
	voxels->velocityX->setCurrentValue(Vector3(4, 4, 0), 44);


	voxels->velocityY->setCurrentValue(Vector3(0, 1, 0), 01);
	voxels->velocityY->setCurrentValue(Vector3(0, 2, 0), 02);
	voxels->velocityY->setCurrentValue(Vector3(0, 3, 0), 03);
	voxels->velocityY->setCurrentValue(Vector3(0, 4, 0), 04);

	voxels->velocityY->setCurrentValue(Vector3(1, 0, 0), 10);
	voxels->velocityY->setCurrentValue(Vector3(1, 1, 0), 11);
	voxels->velocityY->setCurrentValue(Vector3(1, 2, 0), 12);
	voxels->velocityY->setCurrentValue(Vector3(1, 3, 0), 13);
	voxels->velocityY->setCurrentValue(Vector3(1, 4, 0), 14);

	voxels->velocityY->setCurrentValue(Vector3(2, 0, 0), 20);
	voxels->velocityY->setCurrentValue(Vector3(2, 1, 0), 21);
	voxels->velocityY->setCurrentValue(Vector3(2, 2, 0), 22);
	voxels->velocityY->setCurrentValue(Vector3(2, 3, 0), 23);
	voxels->velocityY->setCurrentValue(Vector3(2, 4, 0), 24);

	voxels->velocityY->setCurrentValue(Vector3(3, 0, 0), 30);
	voxels->velocityY->setCurrentValue(Vector3(3, 1, 0), 31);
	voxels->velocityY->setCurrentValue(Vector3(3, 2, 0), 32);
	voxels->velocityY->setCurrentValue(Vector3(3, 3, 0), 33);
	voxels->velocityY->setCurrentValue(Vector3(3, 4, 0), 34);

	voxels->velocityY->setCurrentValue(Vector3(4, 0, 0), 40);
	voxels->velocityY->setCurrentValue(Vector3(4, 1, 0), 41);
	voxels->velocityY->setCurrentValue(Vector3(4, 2, 0), 42);
	voxels->velocityY->setCurrentValue(Vector3(4, 3, 0), 43);
	voxels->velocityY->setCurrentValue(Vector3(4, 4, 0), 44);
}

void CFD::CFDGrid::setDebugDensityValues()
{
	voxels->density->setCurrentValue(Vector3(0, 1, 0), 01);
	voxels->density->setCurrentValue(Vector3(0, 2, 0), 02);
	voxels->density->setCurrentValue(Vector3(0, 3, 0), 03);
	voxels->density->setCurrentValue(Vector3(0, 4, 0), 04);

	voxels->density->setCurrentValue(Vector3(1, 0, 0), 10);
	voxels->density->setCurrentValue(Vector3(1, 1, 0), 11);
	voxels->density->setCurrentValue(Vector3(1, 2, 0), 12);
	voxels->density->setCurrentValue(Vector3(1, 3, 0), 13);
	voxels->density->setCurrentValue(Vector3(1, 4, 0), 14);

	voxels->density->setCurrentValue(Vector3(2, 0, 0), 20);
	voxels->density->setCurrentValue(Vector3(2, 1, 0), 21);
	voxels->density->setCurrentValue(Vector3(2, 2, 0), 22);
	voxels->density->setCurrentValue(Vector3(2, 3, 0), 23);
	voxels->density->setCurrentValue(Vector3(2, 4, 0), 24);

	voxels->density->setCurrentValue(Vector3(3, 0, 0), 30);
	voxels->density->setCurrentValue(Vector3(3, 1, 0), 31);
	voxels->density->setCurrentValue(Vector3(3, 2, 0), 32);
	voxels->density->setCurrentValue(Vector3(3, 3, 0), 33);
	voxels->density->setCurrentValue(Vector3(3, 4, 0), 34);

	voxels->density->setCurrentValue(Vector3(4, 0, 0), 40);
	voxels->density->setCurrentValue(Vector3(4, 1, 0), 41);
	voxels->density->setCurrentValue(Vector3(4, 2, 0), 42);
	voxels->density->setCurrentValue(Vector3(4, 3, 0), 43);
	voxels->density->setCurrentValue(Vector3(4, 4, 0), 44);
}
