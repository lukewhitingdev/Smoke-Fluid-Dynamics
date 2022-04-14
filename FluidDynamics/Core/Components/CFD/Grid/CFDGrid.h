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

	// TODO: Intergrate this with the other classes.
	struct Vector3
	{
		Vector3(int x, int y, int z) : x(x), y(y), z(z) {};
		int x, y, z;

		Vector3 operator - (const Vector3& other)
		{
			return Vector3(x - other.x, y - other.y, z - other.z);
		}

		Vector3	operator -() 
		{
			return Vector3(-x, -y, -z);
		};
	};

	struct Velocity
	{
		Velocity(int x, int y, int z) : x(x), y(y), z(z) {};
		Velocity() : x(0), y(0), z(0) {};
		int x, y, z;

		Velocity operator * (const float& scalar)
		{
			return Velocity(x * scalar, y * scalar, z * scalar);
		}

		void operator = (const Vector3& vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
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
		CFDVoxel() : position(Vector3(-1,-1,-1)), x(-1), y(-1), z(-1), data(new CFDData()) {};
		CFDVoxel(int x, int y, int z) : position(Vector3(x, y, z)), x(x), y(y), z(z), data(new CFDData()) {};

		CFDData* getData() { return data; };

		Vector3 position;
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

		// Velocity Step.
		void updateVelocity(float deltaTime);
		// Diffusion Step.
		void updateDiffuse(float deltaTime);
		// Avection Step.
		void updateAdvection(float deltaTime);
		// Keep fluid within boundary.
		void updateBoundaryVoxels();

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
					printf(" [[D: %f][V: %d, %d, %d]] ", voxel.data->density, voxel.data->velocity.x, voxel.data->velocity.y, voxel.data->velocity.z);
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


