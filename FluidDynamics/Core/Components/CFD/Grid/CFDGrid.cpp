#include "CFDGrid.h"
#include "Utility/Math/Math.h"
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
	if (x > width || y > height || z > depth)
		return;

	CFDVoxel* voxel = this->getVoxel(x, y, z, voxels);
	voxel->data->density = density;
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

void CFD::CFDGrid::updateDiffuse(float deltaTime)
{
	diffuse = deltaTime * diffusionRate * totalVoxels;
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


	for(int i = 0; i < 20; i++) // Seems to be for relaxation but idk.
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

void CFD::CFDGrid::updateAdvection(float deltaTime)
{
	int x0, y0, z0; // Difference between the x,y,z components of the current frame and the previous frame (backtraced?).

	float deltaTime0 = deltaTime * totalVoxels;

	for(int z = 0; z < depth; ++z)
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{

			}
		}	
	}
}

void CFD::CFDGrid::updatePreviousPreviousFrameVoxels()
{
	memcpy_s(voxels0, sizeof(CFDVoxel) * totalVoxels, voxels, sizeof(CFDVoxel) * totalVoxels);
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
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	direct3D->device->CreateSamplerState(&sampDesc, &sampler);
}

void CFDGrid::Update(float deltaTime)
{
	static int iter;
	D3D* direct3D = D3D::getInstance();

	//printf("Post Source: \n");
	//this->printGridInfomation(voxels);

	//system("cls");

	updateDiffuse(0.016f);

	//printf("\n Post Diffusion: \n");
	//this->printGridInfomation(voxels);

	//direct3D->immediateContext->UpdateSubresource(voxelTex, 0, nullptr, voxels, width, depth);
	//printf("Iteration: %d \n", iter);

	// Test, this previous density needs to be equal if its using a stable method since we can backtrace linear methods.
	//printf("Density[1,1,1] Prev: %f \n", this->getDensityPreviousFrame(0, 0, 0));
	//printf("Density[1,1,1] Curr: %f \n", this->getDensity(0, 0, 0));

	printf("[%d] %f \n", iter,this->getDensity(0,0,0));
	//printf("%f \n", this->getVoxel(0, 0, 0, voxels0)->data->density);

	iter++;

	updatePreviousPreviousFrameVoxels();
}

void CFDGrid::Render()
{
	D3D* direct3D = D3D::getInstance();

	direct3D->immediateContext->UpdateSubresource(voxelTex, 0, nullptr, voxels, width, depth);
	direct3D->immediateContext->PSSetSamplers(0, 1, &sampler);
	direct3D->immediateContext->PSSetShaderResources(0, 1, &voxelView);
}
