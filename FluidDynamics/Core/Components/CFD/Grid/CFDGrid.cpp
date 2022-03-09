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

	return &voxels[index];
}

void CFDGrid::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);
}
