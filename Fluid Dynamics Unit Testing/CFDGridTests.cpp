#include "pch.h"
#include <d3dcompiler.h>
#include <Windows.h>

#include "Core/Entities/GameObject.h"
#include "Core/Entities/GameObject.cpp"

#include "Core/Components/Transform/Transform.h"
#include "Core/Components/Transform/Transform.cpp"

#include "Core/Components/Mesh/Mesh.h"
#include "Core/Components/Mesh/Mesh.cpp"

#include "Core/Components/Material/Material.h"
#include "Core/Components/Material/Material.cpp"

#include "Dependencies/Textures/DDSTextureLoader.h"
#include "Dependencies/Textures/DDSTextureLoader.cpp"

#include "Utility/Direct3D/Headers/D3D.h"
#include "Utility/Direct3D/D3D.cpp"

#include "Utility/Window/Headers/Window.h"
#include "Utility/Window/Window.cpp"

#include "Utility/Input System/InputSystem.h"
#include "Utility/Input System/InputSystem.cpp"

#include "Core/Components/CFD/Grid/CFDGrid.h"
#include "Core/Components/CFD/Grid/CFDGrid.cpp"

#include "Utility/Math/Math.h"
#include "Utility/Math/Math.cpp"

/*
TODO:
Make this into a suite so we can re-use device and object to test other things, 
just so that when running the tests we dont have to initDevice and create a gameObject everytime which is slow.
*/

/*------- Grid Functionality Tests ------*/

TEST(CFDGrid, setGridSize) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int x = 5;
	int y = 5;
	int z = 5;

	int targetx = 2;
	int targety = 2;
	int targetz = 4;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();
	grid->setGrid(x, y, z);
	
	CFD::CFDVoxel* voxel = grid->getVoxelCurrentFrame(targetx, targety, targetz);

	EXPECT_TRUE(voxel != nullptr)<< "Voxel could not be found!";
	EXPECT_EQ(voxel->x, targetx) << "Voxel X Coordinate Incorrect!";
	EXPECT_EQ(voxel->y, targety) << "Voxel Y Coordinate Incorrect!";
	EXPECT_EQ(voxel->z, targetz) << "Voxel Z Coordinate Incorrect!";
}

TEST(CFDGrid, getVoxelValid) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int x = 5;
	int y = 5;
	int z = 5;

	int targetx = 2;
	int targety = 2;
	int targetz = 4;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();
	grid->setGrid(x, y, z);

	CFD::CFDVoxel* voxel = grid->getVoxelCurrentFrame(targetx, targety, targetz);

	EXPECT_TRUE(voxel != nullptr) << "Voxel could not be found!";
	EXPECT_EQ(voxel->x, targetx) << "Voxel X Coordinate Incorrect!";
	EXPECT_EQ(voxel->y, targety) << "Voxel Y Coordinate Incorrect!";
	EXPECT_EQ(voxel->z, targetz) << "Voxel Z Coordinate Incorrect!";
}

TEST(CFDGrid, getVoxelInvalid) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int x = 5;
	int y = 5;
	int z = 5;

	int targetx = 10;
	int targety = 2;
	int targetz = 4;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();
	grid->setGrid(x, y, z);

	CFD::CFDVoxel* voxel = grid->getVoxelCurrentFrame(targetx, targety, targetz);

	EXPECT_TRUE(voxel == nullptr);
}

TEST(CFDGrid, setDensitySource) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int x = 1;
	int y = 1;
	int z = 1;
	float value = 10;

	float deltaTime = 0.1f;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();

	grid->setGrid(2, 2, 2);
	grid->addDensitySource(x, y, z, value);
	grid->Update(deltaTime);
	float result = grid->getDensity(x, y, z);
	float expected = 0.886525;

	EXPECT_TRUE(Math::compareFloat(result, expected, 0.000001f));
}

TEST(CFDGrid, setDensitySourceInvalid) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int x = 11;
	int y = 24;
	int z = 32;
	float value = 10;

	float deltaTime = 0.1f;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();

	grid->setGrid(10, 10, 10);
	grid->addDensitySource(x, y, z, value);
	grid->Update(deltaTime);
	float result = grid->getDensity(x, y, z);
	float expected = -1;

	EXPECT_EQ(result, expected);
}

/*----- Stability Tests ------*/

TEST(CFDSimulation, stableDiffuse2D) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int iterations = 100;

	int gX = 4;
	int gY = 4;
	int gZ = 1;

	int x = 0;
	int y = 0;
	int z = 0;
	float value = 10;

	float deltaTime = 0.1f;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();

	grid->setGrid(gX, gY, gZ);

	for(int i = 0; i < iterations; ++i)
	{
		grid->addDensitySource(x, y, z, value);
		grid->Update(deltaTime);
	}

	float result = grid->getDensity(x, y, z);
	float expected = 0.012751;

	EXPECT_TRUE(Math::compareFloat(result, expected, 0.00001f)) << "Result after " << iterations << " iterations: " << result;
}

TEST(CFDSimulation, stableDiffuse3D) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int iterations = 100;

	int gX = 4;
	int gY = 4;
	int gZ = 4;

	int x = 0;
	int y = 0;
	int z = 0;
	float value = 10;

	float deltaTime = 0.1f;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();

	grid->setGrid(gX, gY, gZ);

	for (int i = 0; i < iterations; ++i)
	{
		grid->addDensitySource(x, y, z, value);
		grid->Update(deltaTime);
	}

	float result = grid->getDensity(x, y, z);
	float expected = 0.012751;

	EXPECT_TRUE(Math::compareFloat(result, expected, 0.00001f)) << "Result after " << iterations << " iterations: " << result;
}