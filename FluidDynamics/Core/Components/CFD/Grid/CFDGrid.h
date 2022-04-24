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

		void decreaseCurrentValue(const int index, const T& val) { setCurrentValue(index, getCurrentValue(index) - val); }
		void decreaseCurrentValue(const Vector3& pos, const T& val) { setCurrentValue(pos, getCurrentValue(pos) - val); }

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

		void swapCurrAndPrevArrays() 
		{
			float* tmp = prev;
			prev = curr;
			curr = tmp;
		};
		
	private:

		int getIndex(const Vector3& voxelPos) {
			//int index = int((voxelPos.x) + (N + 2) * (voxelPos.y)); // 2D.
			int index = N * N * voxelPos.z + voxelPos.y * N + voxelPos.x;
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

		void setGrid(const int size, const int dim) {
			N = size;
			dimensions = dim;
			totalN = pow((N+2), 3);
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

		int getGridSize() { return pow(N, dimensions); }
		int getGridWidth() { return N; }
		int getGridHeight() { return N; }
		int getGridDepth() { return N; }

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
			int size = pow(N+2, dimensions);
			for (int i = 0; i < size; i++)
			{
				data->increaseCurrentValue(i, data->getPreviousValue(i) * deltaTime);
			}
		}

		template<typename T>
		void updateDiffusion(VoxelData<T>* data, float boundary, float diff, float deltaTime) 
		{
			float k = deltaTime * diff * pow(N, dimensions);
			float c = (dimensions > 2) ? 1 + 6 * k : 1 + 4 * k;
		
			for (int i = 0; i < 20; i++) 
			{
				for(int x = 0; x < N; x++)
				{
					for (int y = 0; y < N; y++)
					{
						for(int z = 0; z < N; z++)
						{
							float totalSurroundingValues;
							float x0, x1, y0, y1, z0, z1;
							float prev;
							float value;
							if (dimensions > 2)
							{
								totalSurroundingValues = data->getCurrentValue(Vector3(x - 1, y, z)) + data->getCurrentValue(Vector3(x + 1, y, z))
									+ data->getCurrentValue(Vector3(x, y - 1, z)) + data->getCurrentValue(Vector3(x, y + 1, z))
									+ data->getCurrentValue(Vector3(x, y, z - 1)) + data->getCurrentValue(Vector3(x, y, z + 1)); // z.

								x0 = data->getCurrentValue(Vector3(x - 1, y, z));
								x1 = data->getCurrentValue(Vector3(x + 1, y, z));

								y0 = data->getCurrentValue(Vector3(x, y - 1, z));
								y1 = data->getCurrentValue(Vector3(x, y + 1, z));

								z0 = data->getCurrentValue(Vector3(x, y, z - 1));
								z1 = data->getCurrentValue(Vector3(x, y, z + 1));

								prev = data->getPreviousValue(Vector3(x, y, z));

								value = (prev + k * (x0 + x1 + y0 + y1 + z0 + z1)) / c;
							}
							else
							{
								totalSurroundingValues = data->getCurrentValue(Vector3(x - 1, y, z)) + data->getCurrentValue(Vector3(x + 1, y, z))
									+ data->getCurrentValue(Vector3(x, y - 1, z)) + data->getCurrentValue(Vector3(x, y + 1, z)); // z.

								x0 = data->getCurrentValue(Vector3(x - 1, y, z));
								x1 = data->getCurrentValue(Vector3(x + 1, y, z));

								y0 = data->getCurrentValue(Vector3(x, y - 1, z));
								y1 = data->getCurrentValue(Vector3(x, y + 1, z));

								prev = data->getPreviousValue(Vector3(x, y, z));

								value = (prev + k * (x0 + x1 + y0 + y1)) / c;
							}
							data->setCurrentValue(Vector3(x, y, z), value);
						}
					}
				}
				updateCurrentDataBoundary(data, boundary);
			}
		};

		void updateDiffusionAdvection(VoxelData<float>* data, VoxelData<float>* velocityDataX, VoxelData<float>* velocityDataY, VoxelData<float>* velocityDataZ, float boundary, float deltaTime)
		{
			float x0, y0, z0, dt0;
			Vector3 backtracePosition;
			Vector3 absolutePosition;

			dt0 = deltaTime * pow(N, dimensions);

			for (int x = 0; x < N; ++x)
			{
				for (int y = 0; y < N; ++y)
				{
					for (int z = 0; z < N; ++z)
					{
						float a, b;
						float interpX, interpY, interpZ;
						float value;

						if(dimensions > 2)
						{
							backtracePosition = Vector3(float(x - dt0 * velocityDataX->getCurrentValue(Vector3(x, y, z))),
								float(y - dt0 * velocityDataY->getCurrentValue(Vector3(x, y, z))),
								float(z - dt0 * velocityDataZ->getCurrentValue(Vector3(x, y, z))));

							Math::clamp(backtracePosition, 0.5f, N + 0.5f);

							absolutePosition = Vector3(int(backtracePosition.x), int(backtracePosition.y), int(backtracePosition.z));

							// Interpolate between all neighbours

							a = data->getPreviousValue(Vector3(absolutePosition.x + 1, absolutePosition.y, absolutePosition.z));	// Left
							b = data->getPreviousValue(Vector3(absolutePosition.x - 1, absolutePosition.y, absolutePosition.z)); // Right

							interpX = Math::lerp(a, b, backtracePosition.x);

							a = data->getPreviousValue(Vector3(absolutePosition.x, absolutePosition.y + 1, absolutePosition.z));	// up
							b = data->getPreviousValue(Vector3(absolutePosition.x, absolutePosition.y - 1, absolutePosition.z)); // down

							interpY = Math::lerp(a, b, backtracePosition.y);

							a = data->getPreviousValue(Vector3(absolutePosition.x, absolutePosition.y, absolutePosition.z + 1));	// forward
							b = data->getPreviousValue(Vector3(absolutePosition.x, absolutePosition.y, absolutePosition.z - 1)); // back

							interpZ = Math::lerp(a, b, backtracePosition.z);

							value = (interpX + interpY + interpZ);
						}
						else
						{
							backtracePosition = Vector3(float(x - dt0 * velocityDataX->getCurrentValue(Vector3(x, y, z))), 
														float(y - dt0 * velocityDataY->getCurrentValue(Vector3(x, y, z))),
														float(z - dt0 * velocityDataY->getCurrentValue(Vector3(x, y, z))));

							Math::clamp(backtracePosition, 0.5f, N + 0.5f);

							absolutePosition = Vector3(int(backtracePosition.x), int(backtracePosition.y), int(backtracePosition.z));

							// Interpolate between all neighbours

							a = data->getPreviousValue(Vector3(absolutePosition.x + 1, absolutePosition.y, absolutePosition.z));	// Left
							b = data->getPreviousValue(Vector3(absolutePosition.x - 1, absolutePosition.y, absolutePosition.z)); // Right

							interpX = Math::lerp(a, b, backtracePosition.x);

							a = data->getPreviousValue(Vector3(absolutePosition.x, absolutePosition.y + 1, absolutePosition.z));	// up
							b = data->getPreviousValue(Vector3(absolutePosition.x, absolutePosition.y - 1, absolutePosition.z)); // down

							interpY = Math::lerp(a, b, backtracePosition.y);

							value = (interpX + interpY);
						}
						data->setCurrentValue(Vector3(x, y, z), Math::clamp(value, 0.0f, FLT_MAX));
					}
				}
			}
			updateCurrentDataBoundary(data, boundary);
		};

		void updateMassConservation(VoxelData<float>* velocityX, VoxelData<float>* velocityY, VoxelData<float>* velocityZ, float deltaTime)
		{
			for(int x = 0; x < N; x++)
			{
				for (int y = 0; y < N; y++)
				{
					for (int z = 0; z < N; z++)
					{
						float xDiff = velocityX->getCurrentValue(Vector3(x + 1, y, z)) - velocityX->getCurrentValue(Vector3(x - 1, y, z));
						float yDiff = velocityY->getCurrentValue(Vector3(x, y+1, z)) - velocityY->getCurrentValue(Vector3(x, y-1, z));
						float zDiff = velocityZ->getCurrentValue(Vector3(x, y, z+1)) - velocityZ->getCurrentValue(Vector3(x, y, z-1)); // ?

						float value = -0.5f * (xDiff + yDiff + zDiff) / N;

						velocityY->setPreviousValue(Vector3(x, y, z), value);

						velocityX->setPreviousValue(Vector3(x, y, z), 0);
					}
				}
			}

			updatePreviousDataBoundary(velocityX, 0);
			updatePreviousDataBoundary(velocityY, 0);
			updatePreviousDataBoundary(velocityZ, 0);

			float k = 1;
			float c = 4;

			for (int i = 0; i < 20; i++)
			{
				for (int x = 0; x < N; x++)
				{
					for (int y = 0; y < N; y++)
					{
						for (int z = 0; z < N; z++)
						{
							float totalSurroundingValues;
							float x0, x1, y0, y1, z0, z1;
							float prev;
							float value;
							if (dimensions > 2)
							{
								totalSurroundingValues = velocityX->getCurrentValue(Vector3(x - 1, y, z)) + velocityX->getCurrentValue(Vector3(x + 1, y, z))
									+ velocityX->getCurrentValue(Vector3(x, y - 1, z)) + velocityX->getCurrentValue(Vector3(x, y + 1, z))
									+ velocityX->getCurrentValue(Vector3(x, y, z - 1)) + velocityX->getCurrentValue(Vector3(x, y, z + 1)); // z.

								x0 = velocityY->getCurrentValue(Vector3(x - 1, y, z));
								x1 = velocityY->getCurrentValue(Vector3(x + 1, y, z));

								y0 = velocityY->getCurrentValue(Vector3(x, y - 1, z));
								y1 = velocityY->getCurrentValue(Vector3(x, y + 1, z));

								z0 = velocityY->getCurrentValue(Vector3(x, y, z - 1));
								z1 = velocityY->getCurrentValue(Vector3(x, y, z + 1));

								prev = velocityX->getPreviousValue(Vector3(x, y, z));

								value = (prev + k * (x0 + x1 + y0 + y1 + z0 + z1)) / c;
							}
							else
							{
								totalSurroundingValues = velocityX->getCurrentValue(Vector3(x - 1, y, z)) + velocityX->getCurrentValue(Vector3(x + 1, y, z))
									+ velocityX->getCurrentValue(Vector3(x, y - 1, z)) + velocityX->getCurrentValue(Vector3(x, y + 1, z)); // z.

								x0 = velocityY->getCurrentValue(Vector3(x - 1, y, z));
								x1 = velocityY->getCurrentValue(Vector3(x + 1, y, z));

								y0 = velocityY->getCurrentValue(Vector3(x, y - 1, z));
								y1 = velocityY->getCurrentValue(Vector3(x, y + 1, z));

								prev = velocityX->getPreviousValue(Vector3(x, y, z));

								value = (prev + k * (x0 + x1 + y0 + y1)) / c;
							}
							velocityX->setCurrentValue(Vector3(x, y, z), value);
						}
					}
				}
				updateCurrentDataBoundary(velocityX, 0);
			}

			for (int x = 0; x < N; x++)
			{
				for (int y = 0; y < N; y++)
				{
					for (int z = 0; z < N; z++)
					{
						float xDiff = velocityX->getPreviousValue(Vector3(x + 1, y, z)) - velocityX->getPreviousValue(Vector3(x - 1, y, z));
						float yDiff = velocityX->getPreviousValue(Vector3(x, y+1, z)) - velocityX->getPreviousValue(Vector3(x, y+1, z));
						float zDiff = velocityX->getPreviousValue(Vector3(x, y, z+1)) - velocityX->getPreviousValue(Vector3(x, y, z+1));

						velocityX->decreaseCurrentValue(Vector3(x, y, z), 0.5f * N * xDiff);
						velocityY->decreaseCurrentValue(Vector3(x, y, z), 0.5f * N * yDiff);
						velocityZ->decreaseCurrentValue(Vector3(x, y, z), 0.5f * N * zDiff);
					}
				}
			}

			updateCurrentDataBoundary(velocityX, 1);
			updateCurrentDataBoundary(velocityY, 2);
			updateCurrentDataBoundary(velocityZ, 3);
		}

		void updateCurrentDataBoundary(VoxelData<float>* data, int boundary)
		{
			for (int i = 0; i < N; i++) {

				data->setCurrentValue(Vector3(0, i, 0), (boundary == 1) ? -data->getCurrentValue(Vector3(1, i, 0)) : data->getCurrentValue(Vector3(1, i, 0)));
				data->setCurrentValue(Vector3(N + 1, i, 0), (boundary == 1) ? -data->getCurrentValue(Vector3(N, i, 0)) : data->getCurrentValue(Vector3(N, i, 0)));
				data->setCurrentValue(Vector3(i, 0, 0), (boundary == 1) ? -data->getCurrentValue(Vector3(i, 0, 0)) : data->getCurrentValue(Vector3(i, 0, 0)));
				data->setCurrentValue(Vector3(i, N+1, 0), (boundary == 1) ? -data->getCurrentValue(Vector3(i, N, 0)) : data->getCurrentValue(Vector3(i, N, 0)));
			}

			data->setCurrentValue(Vector3(0, 0, 0), 0.5f * (data->getCurrentValue(Vector3(1, 0, 0)) + data->getCurrentValue(Vector3(0, 1, 0))));
			data->setCurrentValue(Vector3(0, N + 1, 0), 0.5f * (data->getCurrentValue(Vector3(1, N + 1, 0)) + data->getCurrentValue(Vector3(0, N, 0))));
			data->setCurrentValue(Vector3(N + 1, 0, 0), 0.5f * (data->getCurrentValue(Vector3(N, 0, 0)) + data->getCurrentValue(Vector3(N + 1, 1, 0))));
			data->setCurrentValue(Vector3(N+1, N+1, 0), 0.5f * (data->getCurrentValue(Vector3(N,N+1,0)) + data->getCurrentValue(Vector3(N+1, N, 0))));
		}

		void updatePreviousDataBoundary(VoxelData<float>* data, int boundary)
		{
			for (int i = 0; i < N; i++) {

				data->setPreviousValue(Vector3(0, i, 0), (boundary == 1) ? -data->getPreviousValue(Vector3(1, i, 0)) : data->getPreviousValue(Vector3(1, i, 0)));
				data->setPreviousValue(Vector3(N + 1, i, 0), (boundary == 1) ? -data->getPreviousValue(Vector3(N, i, 0)) : data->getPreviousValue(Vector3(N, i, 0)));
				data->setPreviousValue(Vector3(i, 0, 0), (boundary == 1) ? -data->getPreviousValue(Vector3(i, 0, 0)) : data->getPreviousValue(Vector3(i, 0, 0)));
				data->setPreviousValue(Vector3(i, N + 1, 0), (boundary == 1) ? -data->getPreviousValue(Vector3(i, N, 0)) : data->getPreviousValue(Vector3(i, N, 0)));
			}

			data->setPreviousValue(Vector3(0, 0, 0), 0.5f * (data->getPreviousValue(Vector3(1, 0, 0)) + data->getPreviousValue(Vector3(0, 1, 0))));
			data->setPreviousValue(Vector3(0, N + 1, 0), 0.5f * (data->getPreviousValue(Vector3(1, N + 1, 0)) + data->getPreviousValue(Vector3(0, N, 0))));
			data->setPreviousValue(Vector3(N + 1, 0, 0), 0.5f * (data->getPreviousValue(Vector3(N, 0, 0)) + data->getPreviousValue(Vector3(N + 1, 1, 0))));
			data->setPreviousValue(Vector3(N + 1, N + 1, 0), 0.5f * (data->getPreviousValue(Vector3(N, N + 1, 0)) + data->getPreviousValue(Vector3(N + 1, N, 0))));
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
		int dimensions = 0;

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


