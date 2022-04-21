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

		T* curr;
		T* prev;
	};

	struct CFDData
	{
		CFDData(const int size)
		{
			density = VoxelData<float>(size);
			velocityX = VoxelData<float>(size);
			velocityY = VoxelData<float>(size);
		};

		VoxelData<float> density;
		VoxelData<float> velocityX;
		VoxelData<float> velocityY;
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
		};

		void Update(float deltaTime);
		void Render();

		void addDensity(const Vector3& pos, const float val) { voxels->density.setPreviousValue(pos, val); }

		void addVelocity(const Vector3& pos, const Vector3& val) { voxels->velocityX.setPreviousValue(pos, val.x); voxels->velocityY.setPreviousValue(pos, val.y); }

	private:
		
		// Simulation Steps.
		void resetValuesForCurrentFrame();

		void densityStep(float deltaTime);

		void velocityStep(float deltaTime);

		void printGrid()
		{
			// Print
			for (int y = 0; y < N; ++y)
			{
				for (int x = 0; x < N; ++x)
				{
					int index = ((x)+(N + 2) * (y));
					printf(" [%f] ", voxels->density.getCurrentArray()[index]);
				}
				printf("\n");
			}
			printf("\n");
		}

		int N;
		const int viscocity = 0.0f;
		const int diffusionRate = 1.0f;

		CFDData* voxels;

		float* densityTextureData;

		// Texture.
		ID3D11SamplerState* sampler;
		ID3D11Texture3D* voxelTex;
		ID3D11Resource* voxelResource;
		ID3D11ShaderResourceView* voxelView;
	};
}


