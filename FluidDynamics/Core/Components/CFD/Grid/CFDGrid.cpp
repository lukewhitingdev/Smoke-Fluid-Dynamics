#include "CFDGrid.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace CFD;

CFDGrid::CFDGrid() : N(0), totalN(0)
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
	texDesc.Depth = (dimensions > 2) ? N : 1;
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
	texDesc.Depth = (dimensions > 2) ? N : 1;
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
		resetValuesForCurrentFrame();

		updateForces();

		addRandomVelocity();

		velocityStep(0.1f);
		densityStep(0.1f);
	}
}

void CFDGrid::Render()
{
	if (simulating)
	{
		D3D* direct3D = D3D::getInstance();

		// Move density data into proper texture format.
		int index = 0;
		for (int z = 0; z < N; ++z)
		{
			for (int y = 0; y < N; ++y)
			{
				for (int x = 0; x < N; ++x)
				{
					densityTextureData[index] = voxels->density->getCurrentValue(Vector3(x, y, z));

					float xVelo, yVelo, zVelo;
					xVelo = voxels->velocityX->getCurrentValue(Vector3(x, y, z));
					yVelo = voxels->velocityY->getCurrentValue(Vector3(x, y, z));
					zVelo = voxels->velocityZ->getCurrentValue(Vector3(x, y, z));

					velocityTextureData[index] = Vector4(xVelo, yVelo, zVelo, 0);
					index++;
				}
			}
		}

		direct3D->immediateContext->UpdateSubresource(voxelDensTex, 0, nullptr, densityTextureData, sizeof(float) * N, (dimensions > 2) ? UINT(pow(sizeof(float), dimensions)) : 0);

		direct3D->immediateContext->UpdateSubresource(voxelVeloTex, 0, nullptr, velocityTextureData, sizeof(Vector4) * N, (dimensions > 2) ? UINT(pow(sizeof(Vector4), dimensions)) : 0);

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
			voxels->velocityZ->getCurrentValue(pos));
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
				voxels->velocityZ->setCurrentValue(Vector3(x, y, z), 0);
			}
		}
	}
}

void CFD::CFDGrid::addRandomVelocity()
{
	if(randomVelocityMinMax > 0)
	{
		int randomX = Math::random(0, getGridWidth());
		int randomY = Math::random(0, getGridHeight());
		int randomZ = Math::random(0, getGridHeight());

		int randomValX = Math::random(-randomVelocityMinMax, randomVelocityMinMax);
		int randomValY = Math::random(-randomVelocityMinMax, randomVelocityMinMax);
		int randomValZ = Math::random(-randomVelocityMinMax, randomVelocityMinMax);

		addVelocity(Vector3(randomX, randomY, (dimensions > 2) ? randomZ : 0), Vector3(randomValX, randomValY, (dimensions > 2) ? randomValZ : 0));
	}
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
		voxels->velocityZ->setCurrentValue(velo.pos, voxels->velocityZ->getPreviousValue(velo.pos) + velo.value.z);
	}
}

void CFD::CFDGrid::densityStep(float deltaTime)
{
	updateFromPreviousFrame(voxels->density, deltaTime);

	voxels->density->swapCurrAndPrevArrays();

	updateDiffusion(voxels->density, 0, diffusionRate, deltaTime);

	voxels->density->swapCurrAndPrevArrays();

	updateAdvection(voxels->density, voxels->velocityX, voxels->velocityY, voxels->velocityZ, 0, deltaTime);
}

void CFD::CFDGrid::velocityStep(float deltaTime)
{
	updateFromPreviousFrame(voxels->velocityX, deltaTime);
	updateFromPreviousFrame(voxels->velocityY, deltaTime);
	updateFromPreviousFrame(voxels->velocityZ, deltaTime);

	voxels->velocityX->swapCurrAndPrevArrays();
	voxels->velocityY->swapCurrAndPrevArrays();
	voxels->velocityZ->swapCurrAndPrevArrays();

	updateDiffusion(voxels->velocityX, 1, viscocity, deltaTime);
	updateDiffusion(voxels->velocityY, 2, viscocity, deltaTime);
	updateDiffusion(voxels->velocityZ, 3, viscocity, deltaTime);

	updateMassConservation(voxels->velocityX, voxels->velocityY, voxels->velocityZ, deltaTime);

	voxels->velocityX->swapCurrAndPrevArrays();
	voxels->velocityY->swapCurrAndPrevArrays();
	voxels->velocityZ->swapCurrAndPrevArrays();

	updateAdvection(voxels->velocityX, voxels->velocityX, voxels->velocityY, voxels->velocityZ, 1, deltaTime);
	updateAdvection(voxels->velocityY, voxels->velocityX, voxels->velocityY, voxels->velocityZ, 2, deltaTime);
	updateAdvection(voxels->velocityZ, voxels->velocityX, voxels->velocityY, voxels->velocityZ, 3, deltaTime);

	updateMassConservation(voxels->velocityX, voxels->velocityY, voxels->velocityZ, deltaTime);
}


void CFD::CFDGrid::setDebugVelocityValues()
{
	for (int x = 0; x < N; ++x)
	{
		for (int y = 0; y < N; ++y)
		{
			for (int z = 0; z < N; ++z)
			{
				std::string concat = std::to_string(x + 1) + std::to_string(y + 1) + std::to_string(z + 1);
				voxels->velocityX->setCurrentValue(Vector3(x, y, z), float(std::stoi(concat)));
				voxels->velocityY->setCurrentValue(Vector3(x, y, z), float(std::stoi(concat)));
				voxels->velocityZ->setCurrentValue(Vector3(x, y, z), float(std::stoi(concat)));
			}
		}
	}
}

void CFD::CFDGrid::setDebugDensityValues()
{
	for(int x = 0; x < N; ++x)
	{
		for (int y = 0; y < N; ++y)
		{
			for (int z = 0; z < N; ++z)
			{
				std::string concat = std::to_string(x+1) + std::to_string(y+1) + std::to_string(z+1);
				voxels->density->setCurrentValue(Vector3(x, y, z), float(std::stoi(concat)));
			}
		}
	}
}
