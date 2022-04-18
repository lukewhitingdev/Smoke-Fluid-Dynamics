#pragma once
#include "Core/Entity System/Component.h"
#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>
#include "Utility/Direct3D/Headers/D3D.h"
#include "Utility/Math/Math.h"

namespace CFD
{

	struct CFDBuffer
	{

	};

	// Data for each CFDVoxel.
	struct CFDData
	{
		CFDData() : density(1), velocity() {};

		float density;
		Vector3 velocity;
	};

	// Voxel for to represent the CFD particle.
	struct CFDVoxel
	{
		CFDVoxel() : position(Vector3(-1,-1,-1)), x(-1), y(-1), z(-1), data(new CFDData()) {};
		CFDVoxel(int x, int y, int z) : position(Vector3(x, y, z)), x(x), y(y), z(z), data(new CFDData()) {};

		CFDData* getData() { return data; };

		Vector3 position;
		int x, y, z;
		CFDData* data;

		CFDVoxel operator - (const Vector3& vec)
		{
			return CFDVoxel(x - vec.x, y - vec.y, z - vec.z);
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
		
		// Adds a velocity source to the grid.
		void addVelocitySource(Vector3 pos, Vector3 val);

		// Return the density value at a given position on the grid.
		float getDensity(int x, int y, int z);

	private:
		CFDVoxel* getVoxel(int x, int y, int z, CFDVoxel* arr);

		// Density Step.
		void densityStep(float deltaTime);

		// Density Diffusion Step.
		void updateDensityDiffuse(float deltaTime);

		// Density Avection Step.
		void updateDensityAdvection(float deltaTime);

		// Velocity Step.
		void velocityStep(float deltaTime);

		// Velocity Diffusion Step.
		void updateVelocityDiffuse(float deltaTime);

		// Velocity Avection Step.
		void updateVelocityAdvection(float deltaTime);

		// Velocity Mass conservation using hodge decomposition.
		void updateMassConservation(float deltaTime);

		// Keep fluid within boundary.
		void updateBoundaryVoxels();

		void updatePreviousPreviousFrameVoxels();

		void updateDensityTextureData();

		float diffusionRate = 1.0f;
		float visc = 0.2f;
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
					printf(" [[D: %f][V: %.2f, %.2f, %.2f]] ", voxel.data->density, voxel.data->velocity.x, voxel.data->velocity.y, voxel.data->velocity.z);
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

		float* densityTextureData;

		// Texture.
		ID3D11SamplerState* sampler;
		ID3D11Texture3D* voxelTex;
		ID3D11Resource* voxelResource;
		ID3D11ShaderResourceView* voxelView;
	};
}


