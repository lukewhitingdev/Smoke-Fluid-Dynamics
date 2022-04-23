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

		VoxelData<T>(int sideSize, int totalSize)
		{
			N = sideSize;
			arraySize = totalSize;
			curr = new T[arraySize];
			prev = new T[arraySize];

			for(int i = 0; i < arraySize; ++i)
			{
				curr[i] = 0;
				prev[i] = 0;
			}
		}

		void increaseCurrentValue(const Vector3& pos, const T& val) { setCurrentValue(pos, getCurrentValue(pos) + val); }
		void increaseCurrentValue(const int index, const T& val) { setCurrentValue(index, getCurrentValue(index) + val); }

		void increasePreviousValue(const Vector3& pos, const T& val) { setPreviousValue(pos, getPreviousValue(pos) + val); }
		void increasePreviousValue(const int index, const T& val) { setPreviousValue(index, getPreviousValue(index) + val); }

		void setCurrentValue(const Vector3& pos, const T& val) 
		{
			int index = getIndex(pos);

			if(index != -1)
			{
				curr[getIndex(pos)] = val;
			}
			else
			{
				// Log.
				printf("Tried to assign a value to a voxel that is out of array range!. : (%f, %f, %f) \n", pos.x, pos.y, pos.z);
			}
		}
		void setCurrentValue(const int index, const T& val) 
		{
			if (index <= arraySize)
			{
				curr[index] = val;
			}
			else
			{
				// Log.
				printf("Tried to assign a value to a voxel index that is out of array range!. : (%d) \n", index);
			}
		}
		T getCurrentValue(const Vector3& pos) 
		{
			int index = getIndex(pos);

			if (index != -1)
			{
				return curr[index];
			}
			else
			{
				// Log.
				printf("Tried to get a voxel value that is out of range! : (%f, %f, %f) \n", pos.x, pos.y, pos.z);
				return T();
			}
		}
		T getCurrentValue(const int index) 
		{
			if (index <= arraySize)
			{
				return curr[index];
			}
			else
			{
				// Log.
				printf("Tried to get a voxel value that is out of range! : (%f) \n", index);
				return T();
			}
		}

		void setPreviousValue(const Vector3& pos, const T& val) 
		{
			int index = getIndex(pos);

			if (index != -1)
			{
				prev[getIndex(pos)] = val;
			}
			else
			{
				// Log.
				printf("Tried to assign a value to a voxel that is out of array range!. : (%f, %f, %f) \n", pos.x, pos.y, pos.z);
			}
		}
		void setPreviousValue(const int index, const T& val) 
		{
			if (index <= arraySize)
			{
				prev[index] = val;
			}
			else
			{
				// Log.
				printf("Tried to assign a value to a voxel index that is out of array range!. : (%d) \n", index);
			}
		}
		T getPreviousValue(const Vector3& pos) 
		{
			int index = getIndex(pos);

			if (index != -1)
			{
				return prev[index];
			}
			else
			{
				// Log.
				printf("Tried to get a voxel value that is out of range! : (%f, %f, %f) \n", pos.x, pos.y, pos.z);
				return T();
			}
		}
		T getPreviousValue(const int index) 
		{
			if (index <= arraySize)
			{
				return prev[index];
			}
			else
			{
				// Log.
				printf("Tried to get a voxel value that is out of range! : (%f) \n", index);
				return T();
			}
		}

		T* getCurrentArray() { return curr; }
		T* getPreviousArray() { return prev; }

		void pushCurrentDataIntoPreviousArray() { memcpy_s(prev, sizeof(T) * arraySize, curr, sizeof(T) * arraySize); };
		
	private:

		int getIndex(const Vector3& voxelPos) {
			int index = int((voxelPos.x) + (N + 2) * (voxelPos.y)); // 2D.
			//int index = (N * N * voxelPos.z + N * voxelPos.y + voxelPos.x);// 3D.
			if (index > arraySize || index < 0)
				return -1;

			return  index;
		};

		int N;

		int arraySize = 0;

		T* curr = nullptr;
		T* prev = nullptr;
	};

	struct CFDData
	{
		CFDData(const int sizeSize, const int totalSize)
		{
			density = new VoxelData<float>(sizeSize, totalSize);
			velocityX = new VoxelData<float>(sizeSize, totalSize);
			velocityY = new VoxelData<float>(sizeSize, totalSize);
			velocityZ = new VoxelData<float>(sizeSize, totalSize);
		};

		VoxelData<float>* density;
		VoxelData<float>* velocityX;
		VoxelData<float>* velocityY;
		VoxelData<float>* velocityZ;
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

	template<typename T>
	struct QueueItem
	{
		QueueItem(Vector3 pos, T val) : pos(pos), value(val) {};
		QueueItem() : pos(), value() {};
		Vector3 pos;
		T value;
	};


	class CFDGrid : public Component
	{
	public:
		CFDGrid();
		~CFDGrid();

		void Start();

		void setGrid(const int size) {
			N = size;
			totalN = (N+2) * (N + 2) * (N + 2);
			voxels = new CFDData(N, totalN);
			densityTextureData = new float[totalN];
			velocityTextureData = new Vector4[totalN];

			queuedDensities.clear();
			queuedVelocities.clear();
		};

		void Update(float deltaTime);
		void Render();

		void addDensity(const Vector3& pos, const float val);
		void addVelocity(const Vector3& pos, const Vector3& val);

		int getGridSize() { return N * N; }
		int getGridWidth() { return N; }
		int getGridHeight() { return N; }
		bool getSimulating() { return simulating; }

		void setDiffusionRate(float val) { diffusionRate = val; }
		float getDiffusionRate() { return diffusionRate; }

		void setViscocity(float val) { viscocity = val; }
		float getViscocity() { return viscocity; }

		void setRandomVelocityMinMax(int val) { randomVelocityMinMax = val; };

		CFDVoxel getVoxel(const Vector3& pos);

	private:
		
		// Simulation Steps.
		void resetValuesForCurrentFrame();

		void addRandomVelocity();

		void updateForces();

		void densityStep(float deltaTime);

		// Density Step Unravel.
		template<typename T>
		void updateFromPreviousFrame(VoxelData<T>* data, float deltaTime)
		{
			int size = (N + 2) * (N + 2) * (N+2);
			for (int i = 0; i < size; i++)
			{
				data->increaseCurrentValue(i, data->getPreviousValue(i) * deltaTime);
			}
		}

		template<typename T>
		void updateDiffusion(VoxelData<T>* data, float boundary, float deltaTime) 
		{
			float k = deltaTime * diffusionRate * N*N;
			float c = 1 + 4 * a;
		
			for (int i = 0; i < 20; i++) 
			{
				for(int x = 1; x <= N; x++)
				{
					for (int y = 1; y <= N; y++)
					{
						float totalSurroundingValues = data->getCurrentValue(Vector3(x - 1, y, 0)) +  data->getCurrentValue(Vector3(x + 1, y, 0))
													 + data->getCurrentValue(Vector3(x, y - 1, 0)) + data->getCurrentValue(Vector3(x, y + 1, 0))
													 + 0; // z.

						float x0, x1, y0, y1;
						
						x0 = data->getCurrentValue(Vector3(x - 1, y, 0));
						x1 = data->getCurrentValue(Vector3(x + 1, y, 0));

						y0 = data->getCurrentValue(Vector3(x, y - 1, 0));
						y1 = data->getCurrentValue(Vector3(x, y + 1, 0));

						float prev = data->getPreviousValue(Vector3(x, y, 0));

						float value = (prev + k*(x0 + x1 + y0 + y1)) / c;

						data->setCurrentValue(Vector3(x, y, 0), value);
					}
				}
				set_bnd(N, boundary, data->getCurrentArray());
			}
		};

		/*
		// TODO: MAKE THIS WORK FOR VELOICTY AND DENSITY. WILL NEED TO SPLIT THE FUNCTIONS HOWEVER BIG SAD :(
		//void updateVelocityAdvection(VoxelData<float>* data, VoxelData<float>* velocityDataX, VoxelData<float>* velocityDataY, VoxelData<float>* velocityDataZ, float boundary, float deltaTime)
		//{
		//	float x0, y0, z0, dt0;
		//	Vector3 backtracePosition;
		//	Vector3 absolutePosition;

		//	dt0 = deltaTime * N;

		//	for (int x = 0; x < N; ++x)
		//	{
		//		for (int y = 0; y < N; ++y)
		//		{
		//			for (int z = 0; z < N; ++z)
		//			{
		//				backtracePosition = Vector3(float(x - dt0 * velocityDataX->getCurrentValue(Vector3(x, y, z))),
		//											float(y - dt0 * velocityDataY->getCurrentValue(Vector3(x, y, z))),
		//											float(z - dt0 * velocityDataZ->getCurrentValue(Vector3(x, y, z))));

		//				Math::clamp(backtracePosition, 0.5f, N + 0.5f);

		//				absolutePosition = Vector3(int(backtracePosition.x), int(backtracePosition.y), int(backtracePosition.z));

		//				// Get Interpolate Values for each axis
		//				float interpX = Math::lerp(data->getPreviousValue(absolutePosition).x,
		//										   data->getPreviousValue(Vector3(absolutePosition.x + 1, absolutePosition.y, absolutePosition.z)).x,
		//										   backtracePosition.x);

		//				float interpY = Math::lerp(data->getPreviousValue(absolutePosition).y,
		//										   data->getPreviousValue(Vector3(absolutePosition.x, absolutePosition.y + 1, absolutePosition.z)).y,
		//										   backtracePosition.y);

		//				float interpZ = Math::lerp(data->getPreviousValue(absolutePosition).z,
		//										   data->getPreviousValue(Vector3(absolutePosition.x, absolutePosition.y, absolutePosition.z + 1)).z,
		//										   backtracePosition.z);

		//				Vector3 value = Vector3(interpX, interpY, interpZ);

		//				data->setCurrentValue(Vector3(x, y, z), value);
		//			}
		//		}
		//	}
		//	set_bnd(N, boundary, data->());
		//};
		*/

		void updateDiffusionAdvection(VoxelData<float>* data, VoxelData<float>* velocityDataX, VoxelData<float>* velocityDataY, VoxelData<float>* velocityDataZ, float boundary, float deltaTime)
		{
			float x0, y0, z0, dt0;
			Vector3 backtracePosition;
			Vector3 absolutePosition;

			dt0 = deltaTime * N * N;

			for (int x = 0; x < N; ++x)
			{
				for (int y = 0; y < N; ++y)
				{
					for (int z = 0; z < N; ++z)
					{
						backtracePosition = Vector3(float(x - dt0 * velocityDataX->getCurrentValue(Vector3(x, y, z))),
							float(y - dt0 * velocityDataY->getCurrentValue(Vector3(x, y, z))),
							float(z - dt0 * velocityDataZ->getCurrentValue(Vector3(x, y, z))));

						Math::clamp(backtracePosition, 0.5f, N + 0.5f);

						absolutePosition = Vector3(int(backtracePosition.x), int(backtracePosition.y), int(backtracePosition.z));

						// Interpolate between all neighbours

						float a = data->getPreviousValue(Vector3(absolutePosition.x + 1, absolutePosition.y, absolutePosition.z));	// Left
						float b = data->getPreviousValue(Vector3(absolutePosition.x - 1, absolutePosition.y, absolutePosition.z)); // Right

						float interpX = Math::lerp(a, b, backtracePosition.x);

						a = data->getPreviousValue(Vector3(absolutePosition.x, absolutePosition.y+1, absolutePosition.z));	// up
						b = data->getPreviousValue(Vector3(absolutePosition.x, absolutePosition.y-1, absolutePosition.z)); // down

						float interpY = Math::lerp(a, b, backtracePosition.y);

						a = data->getPreviousValue(Vector3(absolutePosition.x, absolutePosition.y, absolutePosition.z+1));	// forward
						b = data->getPreviousValue(Vector3(absolutePosition.x, absolutePosition.y, absolutePosition.z-1)); // back

						float interpZ = Math::lerp(a, b, backtracePosition.z);

						float value = (interpX + interpY + interpZ) / 6;

						data->setCurrentValue(Vector3(x, y, z), value);
					}
				}
			}
			set_bnd(N, boundary, data->getCurrentArray());
		};


		#define IX(i,j) ((i)+(N+2)*(j))

		// Holdover
		void set_bnd(int N, int b, float* x)
		{
			int i;

			for (i = 1; i <= N; i++) {
				x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
				x[IX(N + 1, i)] = b == 1 ? -x[IX(N, i)] : x[IX(N, i)];
				x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
				x[IX(i, N + 1)] = b == 2 ? -x[IX(i, N)] : x[IX(i, N)];
			}
			x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
			x[IX(0, N + 1)] = 0.5f * (x[IX(1, N + 1)] + x[IX(0, N)]);
			x[IX(N + 1, 0)] = 0.5f * (x[IX(N, 0)] + x[IX(N + 1, 1)]);
			x[IX(N + 1, N + 1)] = 0.5f * (x[IX(N, N + 1)] + x[IX(N + 1, N)]);
		}

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
		int totalN;
		float viscocity = 0.0f;
		float diffusionRate = 0.5f;
		int randomVelocityMinMax = 0;

		CFDData* voxels = nullptr;
		std::vector<QueueItem<float>> queuedDensities;
		std::vector<QueueItem<Vector3>> queuedVelocities;

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


