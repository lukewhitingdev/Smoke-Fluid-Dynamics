#pragma once
#include "Core/Entity System/Component.h"
#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>
#include "Utility/Direct3D/Headers/D3D.h"
#include "Utility/Math/Math.h"

namespace CFD
{
	// Holds the previous and current data for a energy in the simulation
	struct VoxelData
	{
		VoxelData() : N(0), curr(nullptr), prev(nullptr) {};

		VoxelData(int sideSize, int totalSize)
		{
			N = sideSize;
			arraySize = totalSize;
			curr = new float[arraySize];
			prev = new float[arraySize];

			for(int i = 0; i < arraySize; ++i)
			{
				curr[i] = 0;
				prev[i] = 0;
			}
		}

		~VoxelData()
		{
			delete[] curr;
			delete[] prev;
		}

		// Increases the current value at the passed in position.
		void increaseCurrentValue(const Vector3& pos, const float val) { setCurrentValue(pos, getCurrentValue(pos) + val); }

		// Increases the current value at the passed in index.
		void increaseCurrentValue(const int index, const float val) { setCurrentValue(index, getCurrentValue(index) + val); }

		// Decreases the current value at the passed in index.
		void decreaseCurrentValue(const int index, const float val) { setCurrentValue(index, getCurrentValue(index) - val); }

		// Decreases the current value at the passed in index.
		void decreaseCurrentValue(const Vector3& pos, const float val) { setCurrentValue(pos, getCurrentValue(pos) - val); }

		// Increases the previous value at the passed in position.
		void increasePreviousValue(const Vector3& pos, const float val) { setPreviousValue(pos, getPreviousValue(pos) + val); }

		// Increases the previous value at the passed in index.
		void increasePreviousValue(const int index, const float val) { setPreviousValue(index, getPreviousValue(index) + val); }

		// Sets the current value at the passed in position.
		void setCurrentValue(const Vector3& pos, const float val) 
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

		// Sets the current value at the passed in index.
		void setCurrentValue(const int index, const float val) 
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

		// Gets the current value at the passed in position.
		float getCurrentValue(const Vector3& pos) 
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
				return 0.0f;
			}
		}

		// Gets the current value at the passed in index.
		float getCurrentValue(const int index) 
		{
			if (index <= arraySize)
			{
				return curr[index];
			}
			else
			{
				// Log.
				printf("Tried to get a voxel value that is out of range! : (%d) \n", index);
				return 0.0f;
			}
		}

		// Sets the previous value at the passed in position.
		void setPreviousValue(const Vector3& pos, const float val) 
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

		// Sets the previous value at the passed in index.
		void setPreviousValue(const int index, const float val) 
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

		// Returns the previous value at the passed in position.
		float getPreviousValue(const Vector3& pos) 
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
				return 0.0f;
			}
		}

		// Returns the previous value at the passed in index.
		float getPreviousValue(const int index) 
		{
			if (index <= arraySize)
			{
				return prev[index];
			}
			else
			{
				// Log.
				printf("Tried to get a voxel value that is out of range! : (%d) \n", index);
				return 0.0f;
			}
		}

		// Returns the current array.
		float* getCurrentArray() { return curr; }

		// Returns the previous array.
		float* getPreviousArray() { return prev; }

		// Swaps the current and previous array pointers.
		void swapCurrAndPrevArrays() 
		{
			float* tmp = prev;
			prev = curr;
			curr = tmp;
		};
		
	private:

		// Returns the index in a 1D array from the passed in position.
		int getIndex(const Vector3& voxelPos) {
			int index = int(N * N * voxelPos.z + voxelPos.y * N + voxelPos.x);
			if (index > arraySize || index < 0)
				return -1;

			return  index;
		};

		int N;

		int arraySize = 0;

		float* curr = nullptr;
		float* prev = nullptr;
	};

	// Holds the data the voxels within the simulation
	struct CFDData
	{
		CFDData(const int sizeSize, const int totalSize)
		{
			density = new VoxelData(sizeSize, totalSize);
			velocityX = new VoxelData(sizeSize, totalSize);
			velocityY = new VoxelData(sizeSize, totalSize);
			velocityZ = new VoxelData(sizeSize, totalSize);
		};

		~CFDData()
		{
			delete density;
			delete velocityX;
			delete velocityY;
			delete velocityZ;
		}

		VoxelData* density;
		VoxelData* velocityX;
		VoxelData* velocityY;
		VoxelData* velocityZ;
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

	// Helper struct for forces and other items stored in queues.
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

		// Starts the simulation
		void Start();

		// Sets the simulation grid size.
		void setGrid(const int size, const int dim) {

			if(voxels != nullptr)
			{
				delete voxels;
				delete[] densityTextureData;
				delete[] velocityTextureData;

				if(voxelDensTex) voxelDensTex->Release();
				if (voxelDensView) voxelDensView->Release();
				if (voxelDensResource) voxelDensResource->Release();
				if (voxelVeloTex) voxelVeloTex->Release();
				if (voxelVeloView) voxelVeloView->Release();
				if (voxelVeloResource) voxelVeloResource->Release();
				if (sampler) sampler->Release();

				queuedDensities.clear();
				queuedVelocities.clear();
			}

			N = size;
			dimensions = dim;
			totalN = int(pow((N+2), 3));
			voxels = new CFDData(N, totalN);
			densityTextureData = new float[totalN];
			velocityTextureData = new Vector4[totalN];
		};

		void Update(float deltaTime);
		void Render();

		// Adds density at the passed in position.
		void addDensity(const Vector3& pos, const float val);

		// Adds velocity at the passed in position.
		void addVelocity(const Vector3& pos, const Vector3& val);

		// Returns the total grid size.
		int getGridSize() { return int(pow(N, dimensions)); }
		
		// Returns the grid width.
		int getGridWidth() { return N; }

		// Returns the grid height.
		int getGridHeight() { return N; }

		// Returns the grid depth.
		int getGridDepth() { return N; }

		// Returns whether the simulation is active.
		bool getSimulating() { return simulating; }

		// Sets the diffusion rate of the density in the simulation.
		void setDiffusionRate(float val) { diffusionRate = val; }

		// Returns the diffusion rate of the density in the simulation.
		float getDiffusionRate() { return diffusionRate; }

		// Sets the viscocity of the velocity in the simulation.
		void setViscocity(float val) { viscocity = val; }

		// Returns the viscocity of the velocity in the simulation.
		float getViscocity() { return viscocity; }

		// Sets the random velocity min max used in the turbulence simulation/
		void setRandomVelocityMinMax(int val) { randomVelocityMinMax = val; };

		void setDimensions(int val) { dimensions = val; }
		int getDimensions() { return dimensions; }

		// Returns the voxel at the passed in position.
		CFDVoxel getVoxel(const Vector3& pos);

	private:
		
		// Resets all the current frames values of all data in the simulation.
		void resetValuesForCurrentFrame();

		// Adds random velocity at random points within the simulation, used to simulate turbulence.
		void addRandomVelocity();

		// Adds forces into the simulation from the queued force lists.
		void updateForces();

		// Simulates Density for a timestep.
		void densityStep(float deltaTime);

		// Simulates Velocity for a timestep.
		void velocityStep(float deltaTime);

		// Updates the current frames values incrementally with the previous frames data.
		void updateFromPreviousFrame(VoxelData* data, float deltaTime)
		{
			int size = int(pow(N+2, dimensions));
			for (int i = 0; i < size; i++)
			{
				data->increaseCurrentValue(i, data->getPreviousValue(i) * deltaTime);
			}
		}

		// Updates diffusion for the data passed in, in accordance with the diffusion value passed in.
		void updateDiffusion(VoxelData* data, float boundary, float diff, float deltaTime) 
		{
			/*
			What is going on here:
				- Loop through every voxel in the grid.
				- Search their neighbours and sum their density.
				- Add the ammount of change we want to see from the previous frame (prev + k)
				- Average / Control the ammount of change with c.
				- Repeat to get stable results, since we are using a intertive solver.
			*/

			float k = deltaTime * diff * float(pow(N, dimensions));	// k = ammount of change we want to see in the diffusion step.
			float c = (dimensions > 2) ? 1 + 6 * k : 1 + 4 * k;     // c = overall change, must be more than k's coefficents so 1+4 for 2D (4 coefficients) and 1+6 for 3D
		
			for (int i = 0; i < 20; i++)		// Gauss-Seidel relaxation interative steps.
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
				updateCurrentDataBoundary(data, int(boundary));
			}
		};

		// Updates advection for the data passed in, in accordance with the velocity data passed.
		void updateAdvection(VoxelData* data, VoxelData* velocityDataX, VoxelData* velocityDataY, VoxelData* velocityDataZ, float boundary, float deltaTime)
		{
			/*
			What is going on here:
				- Loop through every voxel in the grid.
				- "Backtrace" the voxel using dt0 (deltaTime of one sim-step)
				- Using the backtraced position interpolate data values from the apporximate neighbours
				- Apply the interpolates data to the current voxel.
			*/

			float dt0;					// Deltatime of one iteration through the whole simulation.
			Vector3 backtracePosition;
			Vector3 absolutePosition;	// Rounded backtrace position.

			dt0 = deltaTime * float(pow(N, dimensions));

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
			updateCurrentDataBoundary(data, int(boundary));
		};

		// Updates the velocity to be mass-conserving using Hodge-decomposition.
		void updateMassConservation(VoxelData* velocityX, VoxelData* velocityY, VoxelData* velocityZ, float deltaTime)
		{
			UNREFERENCED_PARAMETER(deltaTime);

			/*
			What is going on here:
				- I do not understand this too well but it makes the velocity mass-conserving by subtracting the gradient field from the imcrompressible field.
			*/

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

		// Updates the voxel data's current data to enforce a boundary.
		void updateCurrentDataBoundary(VoxelData* data, int boundary)
		{
			/*
			What is going on here:
				- Sets the data to be constrained by boundaries on X,Y,Z.
			*/

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

		// Updates the voxel data's previous data to enforce a boundary.
		void updatePreviousDataBoundary(VoxelData* data, int boundary)
		{
			/*
			What is going on here:
				- Sets the data to be constrained by boundaries on X,Y,Z.
			*/

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

		// Sets all velocity current values to a reflection of their X,Y,Z coords to debug array alignment.
		void setDebugVelocityValues();

		// Sets all density current values to a reflection of their X,Y,Z coords to debug array alignment.
		void setDebugDensityValues();

		bool simulating = false;

		// ------ Simulation Dimensions

		int N;
		int totalN;
		int dimensions = 2;

		// ------ Simulation Variables.

		float viscocity = 0.0f;
		float diffusionRate = 0.5f;
		int randomVelocityMinMax = 0;

		// Data held within the CFD Grid.

		CFDData* voxels = nullptr;

		// ------------ Input Data.

		std::vector<QueueItem<float>> queuedDensities;
		std::vector<QueueItem<Vector3>> queuedVelocities;

		// ------------ Texture Data.

		float* densityTextureData = nullptr;
		Vector4* velocityTextureData = nullptr;

		// ------------ Textures.

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


