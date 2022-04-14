#pragma once
#include "Core/Entity System/Component.h"
#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>
#include "Utility/Direct3D/Headers/D3D.h"

namespace CFD
{

	struct CFDBuffer
	{

	};

	struct Velocity
	{
		Velocity(int x, int y, int z) : x(x), y(y), z(z) {};
		Velocity() : x(10), y(0), z(0) {};
		int x, y, z;

		Velocity operator * (const float& scalar)
		{
			return Velocity(x * scalar, y * scalar, z * scalar);
		}
	};

	// Data for each CFDVoxel.
	struct CFDData
	{
		CFDData() : density(1), velocity() {};

		float density;
		Velocity velocity;
	};

	// Voxel for to represent the CFD particle.
	struct CFDVoxel
	{
		CFDVoxel() : x(-1), y(-1), z(-1), data(new CFDData()) {};
		CFDVoxel(int x, int y, int z) : x(x), y(y), z(z), data(new CFDData()) {};

		CFDData* getData() { return data; };

		int x, y, z;
		CFDData* data;

		CFDVoxel operator - (const Velocity& velo)
		{
			return CFDVoxel(x - velo.x, y - velo.y, z - velo.z);
		}
	};

	class CFDGrid : public Component
	{
	public:
		CFDGrid();
		~CFDGrid();

		// Starts the simulation and locks in the density values setup before-hand.
		void Start();

		// Sets a CFD grid up with Voxels that are manipulated in the update loop.
		void setGrid(long long w, long long h, long long d);

		// Gets a voxel at a given location.
		CFDVoxel* getVoxelCurrentFrame(int x, int y, int z);
		CFDVoxel* getVoxelPreviousFrame(int x, int y, int z);

		void Update(float deltaTime);
		void Render();

		// Adds a density source to the grid.
		void addDensitySource(int x, int y, int z, float density);

		// Return the density value at a given position on the grid.
		float getDensity(int x, int y, int z);

	private:
		CFDVoxel* getVoxel(int x, int y, int z, CFDVoxel* arr);

		// Diffusion Step.
		void updateDiffuse(float deltaTime);
		void updateAdvection(float deltaTime);

		void updatePreviousPreviousFrameVoxels();

		float diffusionRate = 1.0f;
		float diffuse = 0;

		// Gets the index of a specified x,y,z position in a 1D array.
		long long getIndex(int x, int y, int z)
		{
			return width * height * z + height * y + x;
		}

		void printGridInfomation(CFDVoxel* voxels)
		{
			printf("\n");
			for(int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					CFDVoxel voxel = *this->getVoxelCurrentFrame(x, y, 0);
					printf(" [[D: %f][V: %f, %f, %f]] ", voxel.data->density, voxel.data->velocity.x, voxel.data->velocity.y, voxel.data->velocity.z);
				}
				printf("\n");
			}

		}

		void printDensityAtLocation(int x, int y, int z, CFDVoxel* voxels) 
		{
			if (x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < depth)
			{
				CFDVoxel* voxel = &voxels[this->getIndex(x, y, z)];
				printf("[%d, %d, %d] data: [Density: %f] \n", x, y, z, voxel->data->density);
			}
		};

		long long totalVoxels;

		long long width;
		long long height;
		long long depth;

		bool simulating;

		// Grid.
		CFDVoxel* voxels;
		CFDVoxel* voxels0;

		// Texture.
		ID3D11SamplerState* sampler;
		ID3D11Texture3D* voxelTex;
		ID3D11Resource* voxelResource;
		ID3D11ShaderResourceView* voxelView;
	};
}


