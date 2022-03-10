#pragma once
#include "Core/Entity System/Component.h"
#include <vector>
#include <DirectXMath.h>

namespace CFD
{

	struct Density
	{
		Density(int x, int y, int z, float density) : x(x), y(y), z(z), value(density) {};
		Density(int x, int y, int z) : x(x), y(y), z(z), value() {};
		Density() : x(), y(), z(), value() {};
		int x, y, z;
		float value;
	};


	struct CFDData
	{
		CFDData() : density() {};

		float density;
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

		// Adds a density source to the grid.
		void addDensitySource(int x, int y, int z);

		// Adds a density source to the grid.
		void addDensitySource(int x, int y, int z, float density);

		// Return the density value at a given position on the grid.
		float getDensity(int x, int y, int z);

	private:

		// Updates the densities of specified voxels within the grid.
		void updateDensitySources(float deltaTime);

		std::vector<Density> densities;

		// Gets the index of a specified x,y,z position in a 1D array.
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


