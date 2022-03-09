#pragma once
#include "Core/Entity System/Component.h"
#include <vector>

namespace CFD
{
	struct CFDData
	{
		CFDData() {};
	};

	struct CFDVoxel
	{
		CFDVoxel() : x(-1), y(-1), z(-1), data() {};
		CFDVoxel(int x, int y, int z) : x(x), y(y), z(z), data() {};

		CFDData* getData() { return &data; };

		int x, y, z;
		CFDData data;
	};

	class CFDGrid : public Component
	{
	public:
		CFDGrid();
		~CFDGrid();

		// Sets a CFD grid up with Voxels that are manipulated in the update loop.
		void setGrid(long long w, long long h, long long d);

		// Gets a voxel at a given location.
		CFDVoxel* getVoxel(int x, int y, int z);

		void Update(float deltaTime);
		void Render() {};

	private:

		long long getIndex(int x, int y, int z)
		{
			return width * height * z + height * y + x;
		}

		long long totalVoxels;

		long long width;
		long long height;
		long long depth;

		// Grid.
		CFDVoxel* voxels;
	};
}


