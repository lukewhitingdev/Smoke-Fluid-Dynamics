#include "CFDGrid.h"
using namespace CFD;

CFDGrid::CFDGrid() : totalVoxels(), width(), height(), depth(), voxels(nullptr)
{
}

CFDGrid::~CFDGrid()
{
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

	if (x > width || y > height || z > depth)
		return nullptr;

	return &voxels[index];
}

void CFDGrid::Update(float deltaTime)
{
	updateDensitySources(deltaTime);

	printf("Post Source: \n");
	this->printGridInfomation();

	updateDiffuse(deltaTime);

	printf("Post Diffusion: \n");
	this->printGridInfomation();

}

void CFD::CFDGrid::addDensitySource(int x, int y, int z)
{
	densities.emplace_back(Density(x,y,z));
}

void CFD::CFDGrid::addDensitySource(int x, int y, int z, float density)
{
	if (x > width || y > height || z > depth)
		return;

	densities.emplace_back(Density(x, y, z, density));
}

float CFD::CFDGrid::getDensity(int x, int y, int z)
{
	CFDVoxel* voxel = this->getVoxel(x, y, z);
	if(voxel != nullptr)
	{
		return voxel->data.density;
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
			voxel->data.density += density->value * deltaTime;
			density = nullptr;
		}
	}
}

void CFD::CFDGrid::updateDiffuse(float deltaTime)
{
	float diffuse = diffusionRate * deltaTime * totalVoxels;
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

	for(int i = 0; i < 20; i++) // Seems to be for relaxation but idk.
	{
		for(int x = 0; x < width; x++)
		{
			for(int y = 0; y < height; y++)
			{
				for(int z = 0; z < depth; z++)
				{
					center = this->getVoxel(x, y, z);

					left = this->getVoxel(x - 1, y, z);
					right = this->getVoxel(x + 1, y, z);
					up = this->getVoxel(x, y + 1, z);
					down = this->getVoxel(x, y - 1, z);
					front = this->getVoxel(x, y, z + 1);
					back = this->getVoxel(x, y, z - 1);

					// Calculate the diffuse from our neighbours.
					xDiffuse = (left) ? left->data.density : 0 + (right) ? right->data.density : 0;
					yDiffuse = (up) ? up->data.density : 0 + (down) ? down->data.density : 0;
					zDiffuse = (front) ? front->data.density : 0 + (back) ? back->data.density : 0;

					// Add them up.
					totalDiffuse = xDiffuse + yDiffuse + zDiffuse;

					// Assign the current voxels density to its newly calculated one.
					center->data.density = diffuse * totalDiffuse / (1+4*diffuse);
				}
			}
		}
	}
}
