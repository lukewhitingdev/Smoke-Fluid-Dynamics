#include "CFDGrid.h"
using namespace CFD;

CFDGrid::CFDGrid() : totalVoxels(), width(), height(), depth(), simulating(false), voxels(nullptr), voxels0(nullptr)
{
}

CFDGrid::~CFDGrid()
{
}

void CFDGrid::setGrid(long long w, long long h, long long d)
{
	width = w;
	height = h;
	depth = d;

	totalVoxels = width * height * depth;

	voxels = new CFDVoxel[totalVoxels];
	voxels0 = new CFDVoxel[totalVoxels];
	densityTextureData = new float[totalVoxels];

	int index = 0;
	for (int z = 0; z < depth; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if(index < totalVoxels)
					voxels[index] = CFDVoxel(x, y, z);

				++index;
			}
		}
	}
}

CFDVoxel* CFDGrid::getVoxel(int x, int y, int z, CFDVoxel* arr)
{
	long long index = this->getIndex(x, y, z);

	if (index > totalVoxels)
		return nullptr;

	if (x < 0 || y < 0 || z < 0)
		return nullptr;

	if (x >= width || y >= height || z >= depth)
		return nullptr;

	return &arr[index];
}

void CFD::CFDGrid::densityStep(float deltaTime)
{
	updateDensityDiffuse(deltaTime);
	//updateDensityAdvection(deltaTime);
}

CFDVoxel* CFD::CFDGrid::getVoxelCurrentFrame(int x, int y, int z)
{
	return getVoxel(x, y, z, voxels);
}

CFDVoxel* CFD::CFDGrid::getVoxelPreviousFrame(int x, int y, int z)
{
	return getVoxel(x, y, z, voxels0);
}

void CFD::CFDGrid::addDensitySource(int x, int y, int z, float density)
{
	CFDVoxel* voxel = this->getVoxel(x, y, z, voxels);
	voxel->data->density = density;
}

void CFD::CFDGrid::addVelocitySource(Vector3 pos, Vector3 val)
{
	CFDVoxel* voxel = this->getVoxelCurrentFrame(pos.x, pos.y, pos.z);

	if (voxel == nullptr)
		return;

	voxel->data->velocity = val;
}

float CFD::CFDGrid::getDensity(int x, int y, int z)
{
	CFDVoxel* voxel = this->getVoxel(x, y, z, voxels);
	if(voxel != nullptr)
	{
		return voxel->data->density;
	}
	else
	{
		return -1.0f;
	}
}

void CFD::CFDGrid::updateDensityDiffuse(float deltaTime)
{
	diffuse = deltaTime * diffusionRate * totalVoxels * totalVoxels * totalVoxels;
	const float avg = (1 + 6 * diffuse);

	CFDVoxel* center = nullptr;
	CFDVoxel* prevCenter = nullptr;
	CFDVoxel* left = nullptr;
	CFDVoxel* right = nullptr;
	CFDVoxel* up = nullptr;
	CFDVoxel* down = nullptr;
	CFDVoxel* front = nullptr;
	CFDVoxel* back = nullptr;

	float xDiffuse = 0;
	float yDiffuse = 0;
	float zDiffuse = 0;
	float totalAreaDiffuse = 0;
	float previousFrameDensity = 0;

	float leftDens = 0;
	float rightDens = 0;
	float upDens = 0;
	float downDens = 0;
	float forwardDens = 0;
	float backDens = 0;


	for(int i = 0; i < 20; ++i) // Seems to be for relaxation but idk.
	{
		for (int z = 0; z < depth; ++z)
		{
			for(int y = 0; y < height; ++y)
			{
				for(int x = 0; x < width; ++x)
				{
					center = this->getVoxel(x, y, z, voxels);
					prevCenter = this->getVoxel(x, y, z, voxels0);

					left = this->getVoxel(x - 1, y, z, voxels);
					leftDens = (left) ? left->data->density : 0;

					right = this->getVoxel(x + 1, y, z, voxels);
					rightDens = (right) ? right->data->density : 0;

					up = this->getVoxel(x, y + 1, z, voxels);
					upDens = (up) ? up->data->density : 0;

					down = this->getVoxel(x, y - 1, z, voxels);
					downDens = (down) ? down->data->density : 0;

					front = this->getVoxel(x, y, z + 1, voxels);
					forwardDens = (front) ? front->data->density : 0;

					back = this->getVoxel(x, y, z - 1, voxels);
					backDens = (back) ? back->data->density : 0;

					previousFrameDensity = prevCenter->data->density;

					// Calculate the diffuse from our neighbours.
					xDiffuse = leftDens + rightDens;
					yDiffuse = upDens + downDens;
					zDiffuse = forwardDens + backDens;

					// Add them up.
					totalAreaDiffuse = xDiffuse + yDiffuse + zDiffuse;

					// Assign the current voxels density to its newly calculated one.
					center->data->density = (previousFrameDensity + diffuse * totalAreaDiffuse) / avg;

					//printf("Calculated Density [%i, %i] \n", x,y);
				}
			}
		}
	}
}

void CFD::CFDGrid::updateDensityAdvection(float deltaTime)
{
	CFDVoxel backtracedVoxel;	// Voxel that has been backtraced using the velocity field.

	float deltaTime0 = deltaTime * totalVoxels;

	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			for (int z = 0; z < depth; ++z)
			{
				CFDVoxel* currentVoxel = this->getVoxelCurrentFrame(x, y, z);
				Vector3 backtracePos = currentVoxel->position - currentVoxel->data->velocity * deltaTime0;	// Backtrace the current voxel back one time step using the velocity it has.

				backtracePos.x = Math::clamp(backtracePos.x, 0.1f, width - 1);
				backtracePos.y = Math::clamp(backtracePos.y, 0.1f, height - 1);
				backtracePos.z = Math::clamp(backtracePos.z, 0.1f, depth - 1);

				int roundX = int(round(backtracePos.x));
				int roundY = int(round(backtracePos.y));
				int roundZ = int(round(backtracePos.z));

				backtracedVoxel = *this->getVoxelPreviousFrame(roundX, roundY, roundZ);

				// Average its approximate neighbours (x0 being the floor of the x position and x1 being the ceil of the x position).
				float x0, x1, y0, y1, z0, z1;
				float floatPartX, floatPartY, floatPartZ;
				CFDVoxel* voxelX0;
				CFDVoxel* voxelX1;
				CFDVoxel* voxelY0;
				CFDVoxel* voxelY1;
				CFDVoxel* voxelZ0;
				CFDVoxel* voxelZ1;

				voxelX0 = this->getVoxelPreviousFrame(backtracedVoxel.x, backtracedVoxel.y, backtracedVoxel.z);
				voxelX1 = this->getVoxelPreviousFrame(backtracedVoxel.x + 1, backtracedVoxel.y, backtracedVoxel.z);

				voxelY0 = this->getVoxelPreviousFrame(backtracedVoxel.x, backtracedVoxel.y, backtracedVoxel.z);
				voxelY1 = this->getVoxelPreviousFrame(backtracedVoxel.x, backtracedVoxel.y + 1, backtracedVoxel.z);

				voxelZ0 = this->getVoxelPreviousFrame(backtracedVoxel.x, backtracedVoxel.y, backtracedVoxel.z);
				voxelZ1 = this->getVoxelPreviousFrame(backtracedVoxel.x, backtracedVoxel.y, backtracedVoxel.z + 1);

				x0 = (voxelX0) ? voxelX0->data->density : 0;
				x1 = (voxelX1) ? voxelX1->data->density : 0;

				y0 = (voxelY0) ? voxelY0->data->density : 0;
				y1 = (voxelY1) ? voxelY1->data->density : 0;

				z0 = (voxelZ0) ? voxelZ0->data->density : 0;
				z1 = (voxelZ1) ? voxelZ1->data->density : 0;

				float intPart = 0.0f;

				// Interp
				float intepValueX, intepValueY, intepValueZ;

				intepValueX = modff(backtracePos.x, &intPart);
				intepValueY = modff(backtracePos.y, &intPart);
				intepValueZ = modff(backtracePos.z, &intPart);

				float XInterp = Math::lerp(x0, x1, intepValueX);
				float YInterp = Math::lerp(y0, y1, intepValueY);
				float ZInterp = Math::lerp(z0, z1, intepValueZ);

				currentVoxel->data->density = XInterp + YInterp + ZInterp;
			}
		}
	}
}

void CFD::CFDGrid::velocityStep(float deltaTime)
{
	updateVelocityDiffuse(deltaTime);

	//this->printGridInfomation(voxels);

	// TODO: Understand and fix this
	updateMassConservation(deltaTime);


	updateVelocityAdvection(deltaTime);

	//this->printGridInfomation(voxels);
	//updateMassConservation(deltaTime);
}

void CFD::CFDGrid::updateVelocityDiffuse(float deltaTime)
{
	float viscosity = deltaTime * visc * totalVoxels;
	const float avg = (1 + 6 * viscosity);

	CFDVoxel* center = nullptr;
	CFDVoxel* prevCenter = nullptr;
	CFDVoxel* left = nullptr;
	CFDVoxel* right = nullptr;
	CFDVoxel* up = nullptr;
	CFDVoxel* down = nullptr;
	CFDVoxel* front = nullptr;
	CFDVoxel* back = nullptr;

	Vector3 xVelocity = Vector3();
	Vector3 yVelocity = Vector3();
	Vector3 zVelocity = Vector3();
	Vector3 totalAreaVelocity = Vector3();
	Vector3 previousFrameVelocity = Vector3();

	Vector3 leftVelocity = Vector3();
	Vector3 rightVelocity = Vector3();
	Vector3 upVelocity = Vector3();
	Vector3 downVelocity = Vector3();
	Vector3 forwardVelocity = Vector3();
	Vector3 backVelocity = Vector3();


	for (int i = 0; i < 1; i++) // Seems to be for relaxation but idk.
	{
		for (int z = 0; z < depth; ++z)
		{
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					center = this->getVoxel(x, y, z, voxels);
					prevCenter = this->getVoxel(x, y, z, voxels0);

					left = this->getVoxel(x - 1, y, z, voxels);
					leftVelocity = (left) ? left->data->velocity : leftVelocity;

					right = this->getVoxel(x + 1, y, z, voxels);
					rightVelocity = (right) ? right->data->velocity : rightVelocity;

					up = this->getVoxel(x, y + 1, z, voxels);
					upVelocity = (up) ? up->data->velocity : upVelocity;

					down = this->getVoxel(x, y - 1, z, voxels);
					downVelocity = (down) ? down->data->velocity : downVelocity;

					front = this->getVoxel(x, y, z + 1, voxels);
					forwardVelocity = (front) ? front->data->velocity : forwardVelocity;

					back = this->getVoxel(x, y, z - 1, voxels);
					backVelocity = (back) ? back->data->velocity : backVelocity;

					previousFrameVelocity = prevCenter->data->velocity;

					// Calculate the diffuse from our neighbours.
					xVelocity = leftVelocity + rightVelocity;
					yVelocity = upVelocity + downVelocity;
					zVelocity = forwardVelocity + backVelocity;

					// Add them up.
					totalAreaVelocity = xVelocity + yVelocity + zVelocity;

					// Assign the current voxels density to its newly calculated one.
					center->data->velocity = (previousFrameVelocity + totalAreaVelocity * viscosity) / avg;

					//printf("Calculated Density [%i, %i] \n", x,y);
				}
			}
		}
	}
}

void CFD::CFDGrid::updateVelocityAdvection(float deltaTime)
{
	CFDVoxel backtracedVoxel;	// Voxel that has been backtraced using the velocity field.

	float deltaTime0 = deltaTime * totalVoxels;

	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			for (int z = 0; z < depth; ++z)
			{
				CFDVoxel* currentVoxel = this->getVoxelCurrentFrame(x, y, z);
				Vector3 backtracePos = currentVoxel->position - currentVoxel->data->velocity * deltaTime0;	// Backtrace the current voxel back one time step using the velocity it has.

				backtracePos.x = Math::clamp(backtracePos.x, 0.1f, width - 1);
				backtracePos.y = Math::clamp(backtracePos.y, 0.1f, height - 1);
				backtracePos.z = Math::clamp(backtracePos.z, 0.1f, depth - 1);

				int roundX = int(round(backtracePos.x));
				int roundY = int(round(backtracePos.y));
				int roundZ = int(round(backtracePos.z));

				backtracedVoxel = *this->getVoxelPreviousFrame(roundX, roundY, roundZ);

				// Average its approximate neighbours (x0 being the floor of the x position and x1 being the ceil of the x position).
				float x0, x1, y0, y1, z0, z1;
				float floatPartX, floatPartY, floatPartZ;
				CFDVoxel* voxelX0;
				CFDVoxel* voxelX1;
				CFDVoxel* voxelY0;
				CFDVoxel* voxelY1;
				CFDVoxel* voxelZ0;
				CFDVoxel* voxelZ1;

				voxelX0 = this->getVoxelPreviousFrame(backtracedVoxel.x, backtracedVoxel.y, backtracedVoxel.z);
				voxelX1 = this->getVoxelPreviousFrame(backtracedVoxel.x + 1, backtracedVoxel.y, backtracedVoxel.z);

				voxelY0 = this->getVoxelPreviousFrame(backtracedVoxel.x, backtracedVoxel.y, backtracedVoxel.z);
				voxelY1 = this->getVoxelPreviousFrame(backtracedVoxel.x, backtracedVoxel.y + 1, backtracedVoxel.z);

				voxelZ0 = this->getVoxelPreviousFrame(backtracedVoxel.x, backtracedVoxel.y, backtracedVoxel.z);
				voxelZ1 = this->getVoxelPreviousFrame(backtracedVoxel.x, backtracedVoxel.y, backtracedVoxel.z + 1);

				x0 = (voxelX0) ? voxelX0->data->velocity.x : 0;
				x1 = (voxelX1) ? voxelX1->data->velocity.x : 0;

				y0 = (voxelY0) ? voxelY0->data->velocity.y : 0;
				y1 = (voxelY1) ? voxelY1->data->velocity.y: 0;

				z0 = (voxelZ0) ? voxelZ0->data->velocity.z : 0;
				z1 = (voxelZ1) ? voxelZ1->data->velocity.z : 0;

				float intPart = 0.0f;

				// Interp

				float intepValueX, intepValueY, intepValueZ;

				intepValueX = modff(backtracePos.x, &intPart);
				intepValueY = modff(backtracePos.y, &intPart);
				intepValueZ = modff(backtracePos.z, &intPart);

				float XInterp = Math::lerp(x0, x1, intepValueX);
				float YInterp = Math::lerp(y0, y1, intepValueY);
				float ZInterp = Math::lerp(z0, z1, intepValueZ);

				currentVoxel->data->velocity = Vector3(XInterp, YInterp, ZInterp);
			}
		}
	}
}

void CFD::CFDGrid::updateMassConservation(float deltaTime)
{
	float invTotalVoxels = 1.0 / totalVoxels;

	CFDVoxel* center = nullptr;
	CFDVoxel* prevCenter = nullptr;
	CFDVoxel* left = nullptr;
	CFDVoxel* right = nullptr;
	CFDVoxel* up = nullptr;
	CFDVoxel* down = nullptr;
	CFDVoxel* front = nullptr;
	CFDVoxel* back = nullptr;

	Vector3 xVelocity = Vector3();
	Vector3 yVelocity = Vector3();
	Vector3 zVelocity = Vector3();
	Vector3 totalAreaVelocity = Vector3();
	Vector3 previousFrameVelocity = Vector3();

	Vector3 leftVelocity = Vector3();
	Vector3 rightVelocity = Vector3();
	Vector3 upVelocity = Vector3();
	Vector3 downVelocity = Vector3();
	Vector3 forwardVelocity = Vector3();
	Vector3 backVelocity = Vector3();


	for (int z = 0; z < depth; ++z)
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				center = this->getVoxel(x, y, z, voxels);
				prevCenter = this->getVoxel(x, y, z, voxels0);

				left = this->getVoxel(x - 1, y, z, voxels);
				leftVelocity = (left) ? left->data->velocity : leftVelocity;

				right = this->getVoxel(x + 1, y, z, voxels);
				rightVelocity = (right) ? right->data->velocity : rightVelocity;

				up = this->getVoxel(x, y + 1, z, voxels);
				upVelocity = (up) ? up->data->velocity : upVelocity;

				down = this->getVoxel(x, y - 1, z, voxels);
				downVelocity = (down) ? down->data->velocity : downVelocity;

				front = this->getVoxel(x, y, z + 1, voxels);
				forwardVelocity = (front) ? front->data->velocity : forwardVelocity;

				back = this->getVoxel(x, y, z - 1, voxels);
				backVelocity = (back) ? back->data->velocity : backVelocity;

				previousFrameVelocity = prevCenter->data->velocity;

				// Calculate the velocity from our neighbours.
				xVelocity = leftVelocity + rightVelocity;
				yVelocity = upVelocity + downVelocity;
				zVelocity = forwardVelocity + backVelocity;

				// Subtract them.
				totalAreaVelocity = xVelocity - yVelocity - zVelocity;

				// Assign the current voxels density to its newly calculated one.
				center->data->velocity = totalAreaVelocity * (-0.5 * invTotalVoxels);
			}
		}
	}

	this->updateVelocityDiffuse(deltaTime);

	for (int z = 0; z < depth; ++z)
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				center = this->getVoxel(x, y, z, voxels);
				prevCenter = this->getVoxel(x, y, z, voxels0);

				left = this->getVoxel(x - 1, y, z, voxels);
				leftVelocity = (left) ? left->data->velocity : leftVelocity;

				right = this->getVoxel(x + 1, y, z, voxels);
				rightVelocity = (right) ? right->data->velocity : rightVelocity;

				up = this->getVoxel(x, y + 1, z, voxels);
				upVelocity = (up) ? up->data->velocity : upVelocity;

				down = this->getVoxel(x, y - 1, z, voxels);
				downVelocity = (down) ? down->data->velocity : downVelocity;

				front = this->getVoxel(x, y, z + 1, voxels);
				forwardVelocity = (front) ? front->data->velocity : forwardVelocity;

				back = this->getVoxel(x, y, z - 1, voxels);
				backVelocity = (back) ? back->data->velocity : backVelocity;

				previousFrameVelocity = prevCenter->data->velocity;

				// Calculate the velocity from our neighbours.
				xVelocity = leftVelocity + rightVelocity;
				yVelocity = upVelocity + downVelocity;
				zVelocity = forwardVelocity + backVelocity;

				// Subtract them.
				totalAreaVelocity = xVelocity - yVelocity - zVelocity;

				// Assign the current voxels density to its newly calculated one.
				center->data->velocity = (totalAreaVelocity * 0.5) / invTotalVoxels;
			}
		}
	}
}

void CFD::CFDGrid::updateBoundaryVoxels()
{
	CFDVoxel* minVoxel = nullptr;
	CFDVoxel* maxVoxel = nullptr;

	// TODO: Make Boundary conditions.
	// I.E. Like making the velocity turn inwards once at the boundary to keep it in bounds.

	// X Restraint.
	for(int z = 0; z < depth; ++z) // across
	{
		for (int y = 0; y < height; ++y) // up
		{
			maxVoxel = this->getVoxelCurrentFrame(width-1, y, z);
			minVoxel = this->getVoxelCurrentFrame(0+1, y, z);

			if (width > 1)
			{
				minVoxel = this->getVoxelCurrentFrame(0 + 1, y, z);
				minVoxel->data->velocity.x = 0;
			}

			maxVoxel->data->velocity.x = 0;
		}
	}

	// Y Restraint.
	for (int x = 0; x < width; ++x) // across
	{
		for (int z = 0; z < depth; ++z) // up
		{
			maxVoxel = this->getVoxelCurrentFrame(x, height-1, z);
			minVoxel = this->getVoxelCurrentFrame(x, 0+1, z);

			if (height > 1)
			{
				minVoxel = this->getVoxelCurrentFrame(x, 0 + 1, z);
				minVoxel->data->velocity.y = 0;
			}

			maxVoxel->data->velocity.y = 0;
		}
	}

	// Z Restraint.
	for (int x = 0; x < width; ++x) // across
	{
		for (int y = 0; y < height; ++y) // up
		{
			maxVoxel = this->getVoxelCurrentFrame(x, y, depth-1);

			if(depth > 1)
			{
				minVoxel = this->getVoxelCurrentFrame(x, y, 0+1);	
				minVoxel->data->velocity.z = 0;
			}

			maxVoxel->data->velocity.z = 0;
		}
	}
}

void CFD::CFDGrid::updatePreviousPreviousFrameVoxels()
{
	memcpy_s(voxels0, sizeof(CFDVoxel) * totalVoxels, voxels, sizeof(CFDVoxel) * totalVoxels);
}

void CFD::CFDGrid::updateDensityTextureData()
{
	for(int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			for (int z = 0; z < depth; ++z)
			{
				densityTextureData[this->getIndex(x, y, z)] = this->getVoxelCurrentFrame(x, y, z)->data->density;
			}
		}
	}

	//for (int x = 0; x < width; ++x)
	//{
	//	for (int y = 0; y < height; ++y)
	//	{
	//		for (int z = 0; z < depth; ++z)
	//		{
	//			printf("[%d, %d, %d] %f \n",x,y,z,densityTextureData[this->getIndex(x, y, z)]);
	//		}
	//	}
	//}
}

void CFDGrid::Start()
{
	D3D* direct3D = D3D::getInstance();

	D3D11_TEXTURE3D_DESC texDesc = {};
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE3D_DESC));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.Depth = depth;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32_FLOAT;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(direct3D->device->CreateTexture3D(&texDesc, nullptr, &voxelTex)))
		throw;

	D3D11_SHADER_RESOURCE_VIEW_DESC texViewDesc = {};
	ZeroMemory(&texViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	texViewDesc.Format = texDesc.Format;
	texViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	texViewDesc.Texture2D.MipLevels = texDesc.MipLevels;
	texViewDesc.Texture2D.MostDetailedMip = 0;

	direct3D->device->CreateShaderResourceView(voxelTex, &texViewDesc, &voxelView);

	// Create Sampler
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	direct3D->device->CreateSamplerState(&sampDesc, &sampler);
}

void CFDGrid::Update(float deltaTime)
{

	updatePreviousPreviousFrameVoxels();

	//velocityStep(0.016f);

	densityStep(0.016f);

	updateBoundaryVoxels();

	updateDensityTextureData();

	//this->printGridInfomation(voxels);
	//printf("\n");
}

void CFDGrid::Render()
{
	D3D* direct3D = D3D::getInstance();

	direct3D->immediateContext->UpdateSubresource(voxelTex, 0, nullptr, densityTextureData, sizeof(float)*width, sizeof(float)*(width*height)*depth);
	direct3D->immediateContext->PSSetSamplers(0, 1, &sampler);
	direct3D->immediateContext->PSSetShaderResources(0, 1, &voxelView);
}
