#pragma once
#include "Core/Entity System/Component.h"
#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>
#include "Utility/Direct3D/Headers/D3D.h"
#include "Utility/Math/Math.h"

namespace CFD
{
	template<typename T>
	struct VoxelData
	{
		VoxelData<T>() : curr(nullptr), prev(nullptr) {};

		VoxelData<T>(int size)
		{
			N = size;
			int arraySize = (N + 2) * (N + 2);
			curr = new T[arraySize];
			prev = new T[arraySize];

			for(int i = 0; i < arraySize; ++i)
			{
				curr[i] = 0;
				prev[i] = 0;
			}
		}

		void setCurrentValue(const Vector3& pos, const T& val) { curr[getIndex(pos)] = val; }
		T getCurrentValue(const Vector3& pos) { return curr[getIndex(pos)]; }

		void setPreviousValue(const Vector3& pos, const T& val) { prev[getIndex(pos)] = val; }
		T getPreviousValue(const Vector3& pos) { return prev[getIndex(pos)]; }

		float* getCurrentArray() { return curr; }
		float* getPreviousArray() { return prev; }
		
	private:

		int getIndex(const Vector3& voxelPos) {
			//return (N * N * voxelPos.z + N * voxelPos.y + voxelPos.x); // 3D.
			return ((voxelPos.x)+ (N + 2) * (voxelPos.y)); // 2D.
		};

		int N;

		T* curr = nullptr;
		T* prev = nullptr;
	};

	struct CFDData
	{
		CFDData(const int size)
		{
			density = new VoxelData<float>(size);
			velocityX = new VoxelData<float>(size);
			velocityY = new VoxelData<float>(size);
		};

		VoxelData<float>* density;
		VoxelData<float>* velocityX;
		VoxelData<float>* velocityY;
	};

	// Helper struct to contain general data about the voxel for editing through UI.
	struct CFDVoxel
	{
		CFDVoxel(Vector3 pos, float dens, Vector3 velo) : position(pos), density(dens), velocity(velo) {};
		CFDVoxel() : position(), density(), velocity() {};
		Vector3 position;
		float density;
		Vector3 velocity;
	};

	class CFDGrid : public Component
	{
	public:
		CFDGrid();
		~CFDGrid();

		void Start();

		void setGrid(const int size) {
			N = size; 
			voxels = new CFDData(N);
			densityTextureData = new float[N*N];
			velocityTextureData = new Vector4[N * N];
		};

		void Update(float deltaTime);
		void Render();

		void addDensity(const Vector3& pos, const float val) { voxels->density->setPreviousValue(pos, val); }

		void addVelocity(const Vector3& pos, const Vector3& val) { voxels->velocityX->setPreviousValue(pos, val.x); voxels->velocityY->setPreviousValue(pos, val.y); }

		int getGridSize() { return N * N; }
		int getGridWidth() { return N; }
		int getGridHeight() { return N; }
		void setDiffusionRate(float val) { diffusionRate = val; }
		void setViscocity(float val) { viscocity = val; }

		CFDVoxel getVoxel(const Vector3& pos);

	private:
		
		// Simulation Steps.
		void resetValuesForCurrentFrame();

		void densityStep(float deltaTime);

		void velocityStep(float deltaTime);

		void printGrid()
		{
			// Print
			for (int y = 0; y <= N; ++y)
			{
				for (int x = 0; x <= N; ++x)
				{
					printf("(%d, %d) [%f] [%f,%f] ", x,y, voxels->density->getCurrentValue(Vector3(x,y,0)), 
														  voxels->velocityX->getCurrentValue(Vector3(x,y,0)),
														  voxels->velocityY->getCurrentValue(Vector3(x, y, 0)));
				}
				printf("\n");
			}
			printf("\n");
		}

		void setDebugVelocityValues();
		void setDebugDensityValues();

		bool simulating = false;

		int N;
		int viscocity = 0.0f;
		int diffusionRate = 0.0f;

		CFDData* voxels = nullptr;

		float* densityTextureData = nullptr;
		Vector4* velocityTextureData = nullptr;

		// Texture.
		ID3D11SamplerState* sampler = nullptr;

		// Density
		ID3D11Texture3D* voxelDensTex = nullptr;
		ID3D11Resource* voxelDensResource = nullptr;
		ID3D11ShaderResourceView* voxelDensView = nullptr;

		// Velocity
		ID3D11Texture3D* voxelVeloTex = nullptr;
		ID3D11Resource* voxelVeloResource = nullptr;
		ID3D11ShaderResourceView* voxelVeloView = nullptr;
	};
}


