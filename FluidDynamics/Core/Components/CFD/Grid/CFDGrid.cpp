#include "CFDGrid.h"
#include "Utility/Math/Math.h"
using namespace CFD;

CFDGrid::CFDGrid() : totalVoxels(), width(), height(), depth(), simulating(false), voxels(nullptr), initialVoxels(nullptr)
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
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.Depth = depth;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32_FLOAT;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	if(FAILED(direct3D->device->CreateTexture3D(&texDesc, nullptr, &voxelTex)))
		throw;

	direct3D->immediateContext->UpdateSubresource(voxelTex, 0, nullptr, voxels, width, depth);

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


	updateDensitySources(0.016f);

	initialVoxels = new CFDVoxel[totalVoxels];

	diffuse = diffusionRate * 0.16f * totalVoxels;

	printf("Diffuse: %f", diffuse);

	CFDVoxel* center = nullptr;
	CFDVoxel* left = nullptr;
	CFDVoxel* right = nullptr;
	CFDVoxel* up = nullptr;
	CFDVoxel* down = nullptr;
	CFDVoxel* front = nullptr;
	CFDVoxel* back = nullptr;

	float xDiffuse = 0;
	float yDiffuse = 0;
	float zDiffuse = 0;
	float totalDiffuse = 0;
	float initialDensity = 0;

	float leftDens = 0;
	float rightDens = 0;
	float upDens = 0;
	float downDens = 0;
	float forwardDens = 0;
	float backDens = 0;


	for (int i = 0; i < 1; i++) // Seems to be for relaxation but idk.
	{
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int z = 0; z < depth; z++)
				{
					center = this->getVoxel(x, y, z);

					left = this->getVoxel(x - 1, y, z);
					leftDens = (left) ? left->data->density : 0;

					right = this->getVoxel(x + 1, y, z);
					rightDens = (right) ? right->data->density : 0;

					up = this->getVoxel(x, y + 1, z);
					upDens = (up) ? up->data->density : 0;

					down = this->getVoxel(x, y - 1, z);
					downDens = (down) ? down->data->density : 0;

					front = this->getVoxel(x, y, z + 1);
					forwardDens = (front) ? front->data->density : 0;

					back = this->getVoxel(x, y, z - 1);
					backDens = (back) ? back->data->density : 0;

					// Calculate the diffuse from our neighbours.
					xDiffuse = leftDens + rightDens;
					yDiffuse = upDens + downDens;
					zDiffuse = forwardDens + backDens;

					// Add them up.
					totalDiffuse = xDiffuse + yDiffuse + zDiffuse;

					initialVoxels[this->getIndex(x, y, z)].data->density = Math::clamp(center->data->density - diffuse * (totalDiffuse - 4 * center->data->density), 0.0f, FLT_MAX);
				}
			}
		}
	}


	printf("\n Voxels \n");
	this->printGridInfomation(voxels);

	printf("Initial Voxels \n");
	this->printGridInfomation(initialVoxels);


	simulating = true;
}

void CFDGrid::setGrid(long long w, long long h, long long d)
{
	width = w;
	height = h;
	depth = d;

	totalVoxels = width * height * depth;

	voxels = new CFDVoxel[totalVoxels];

	int index = 0;
	for (int z = 0; z < depth; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if(index < totalVoxels)
					voxels[index] = CFDVoxel(x, y, z);

				++index;
			}
		}
	}
}

CFDVoxel* CFDGrid::getVoxel(int x, int y, int z)
{
	long long index = this->getIndex(x, y, z);

	if (index > totalVoxels)
		return nullptr;

	if (x < 0 || y < 0 || z < 0)
		return nullptr;

	if (x >= width || y >= height || z >= depth)
		return nullptr;

	return &voxels[index];
}

void CFDGrid::Update(float deltaTime)
{
	D3D* direct3D = D3D::getInstance();

	updateDensitySources(0.016f);

	//printf("Post Source: \n");
	//this->printGridInfomation(voxels);

	updateDiffuse(0.016f); 

	printf("\n Post Diffusion: \n");
	this->printGridInfomation(voxels);
}

void CFDGrid::Render()
{
	D3D* direct3D = D3D::getInstance();

	direct3D->immediateContext->UpdateSubresource(voxelTex, 0, nullptr, voxels, width, depth);
	direct3D->immediateContext->PSSetSamplers(0, 1, &sampler);
	direct3D->immediateContext->PSSetShaderResources(0, 1, &voxelView);
}

void CFD::CFDGrid::addDensitySource(int x, int y, int z)
{
	if (!simulating)
		densities.emplace_back(Density(x, y, z));
	else
		printf("Cannot add density whilst simulation is running, please add density at (%d, %d %d) before starting.", x, y, z);
}

void CFD::CFDGrid::addDensitySource(int x, int y, int z, float density)
{
	if(!simulating)
	{
		if (x > width || y > height || z > depth)
			return;

		densities.emplace_back(Density(x, y, z, density));
	}else
	{
		printf("Cannot add density whilst simulation is running, please add density at (%d, %d %d)(dens: %f) before starting.", x, y, z, density);
	}

}

float CFD::CFDGrid::getDensity(int x, int y, int z)
{
	CFDVoxel* voxel = this->getVoxel(x, y, z);
	if(voxel != nullptr)
	{
		return voxel->data->density;
	}
	else
	{
		return -1.0f;
	}
}

void CFD::CFDGrid::updateDensitySources(float deltaTime)
{
	for(int i = 0; i < densities.size(); i++)
	{
		Density* density = &densities[i];
		CFDVoxel* voxel = this->getVoxel(density->x, density->y, density->z);
		if (voxel != nullptr)
		{
			if(voxel->data->density < density->value)
				voxel->data->density += density->value * deltaTime;
		}
	}
}

void CFD::CFDGrid::updateDiffuse(float deltaTime)
{
	CFDVoxel* center = nullptr;
	CFDVoxel* left = nullptr;
	CFDVoxel* right = nullptr;
	CFDVoxel* up = nullptr;
	CFDVoxel* down = nullptr;
	CFDVoxel* front = nullptr;
	CFDVoxel* back = nullptr;

	float xDiffuse = 0;
	float yDiffuse = 0;
	float zDiffuse = 0;
	float totalDiffuse = 0;
	float initialDensity = 0;

	float leftDens = 0;
	float rightDens = 0;
	float upDens = 0;
	float downDens = 0;
	float forwardDens = 0;
	float backDens = 0;


	for(int i = 0; i < 1; i++) // Seems to be for relaxation but idk.
	{
		for(int x = 0; x < width; x++)
		{
			for(int y = 0; y < height; y++)
			{
				for(int z = 0; z < depth; z++)
				{
					center = this->getVoxel(x, y, z);

					left = this->getVoxel(x - 1, y, z);
					leftDens = (left) ? left->data->density : 0;

					right = this->getVoxel(x + 1, y, z);
					rightDens = (right) ? right->data->density : 0;

					up = this->getVoxel(x, y + 1, z);
					upDens = (up) ? up->data->density : 0;

					down = this->getVoxel(x, y - 1, z);
					downDens = (down) ? down->data->density : 0;

					front = this->getVoxel(x, y, z + 1);
					forwardDens = (front) ? front->data->density : 0;

					back = this->getVoxel(x, y, z - 1);
					backDens = (back) ? back->data->density : 0;

					initialDensity = initialVoxels[this->getIndex(x, y, z)].data->density;

					// Calculate the diffuse from our neighbours.
					xDiffuse = leftDens + rightDens;
					yDiffuse = upDens + downDens;
					zDiffuse = forwardDens + backDens;

					// Add them up.
					totalDiffuse = initialDensity + xDiffuse + yDiffuse + zDiffuse;

					// Assign the current voxels density to its newly calculated one.
					center->data->density = Math::clamp(diffuse * totalDiffuse / (1+4*diffuse), 0.0f, FLT_MAX);
				}
			}
		}
	}
}
