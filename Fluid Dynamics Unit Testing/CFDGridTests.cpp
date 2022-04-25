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

#include "Dependencies\UI\IMGUI\imgui.h"
#include "Dependencies\UI\IMGUI\imgui.cpp"

#include "Dependencies\UI\IMGUI\imgui_impl_dx11.h"
#include "Dependencies\UI\IMGUI\imgui_impl_dx11.cpp"

#include "Dependencies\UI\IMGUI\imgui_impl_win32.h"
#include "Dependencies\UI\IMGUI\imgui_impl_win32.cpp"

#include "Dependencies\UI\IMGUI\imgui_internal.h"
#include "Dependencies\UI\IMGUI\imstb_rectpack.h"
#include "Dependencies\UI\IMGUI\imstb_textedit.h"
#include "Dependencies\UI\IMGUI\imstb_truetype.h"
#include "Dependencies\UI\IMGUI\imconfig.h"

#include "Dependencies\UI\IMGUI\imgui_draw.cpp"
#include "Dependencies\UI\IMGUI\imgui_tables.cpp"
#include "Dependencies\UI\IMGUI\imgui_widgets.cpp"



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

	int size = 5;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();
	grid->setGrid(size, 2);
	
	CFD::CFDData* voxels = grid->getAllVoxelData();

	EXPECT_TRUE(voxels->density != nullptr) << "Density is NULL on grid creation!";
	EXPECT_TRUE(voxels->velocityX != nullptr) << "VelocityX is NULL on grid creation!";
	EXPECT_TRUE(voxels->velocityY != nullptr) << "VelocityY is NULL on grid creation!";
	EXPECT_TRUE(voxels->velocityZ != nullptr)<< "VelocityZ is NULL on grid creation!";
}

TEST(CFDGrid, setDensityRoot2D) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int size = 5;

	int targetx = 0;
	int targety = 0;
	int targetz = 0;

	int targetValue = 204;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();
	grid->setGrid(size, 2);

	grid->getAllVoxelData()->density->setCurrentValue(Vector3(targetx, targety, targetz), targetValue);
	int expected = int(grid->getAllVoxelData()->density->getCurrentValue(Vector3(targetx, targety, targetz)));

	EXPECT_EQ(targetValue, expected) << "Setting Density at a point 2D does not set properly!";
}

TEST(CFDGrid, setDensityRoot3D) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int size = 5;

	int targetx = 0;
	int targety = 0;
	int targetz = 0;

	int targetValue = 204;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();
	grid->setGrid(size, 3);

	grid->getAllVoxelData()->density->setCurrentValue(Vector3(targetx, targety, targetz), targetValue);
	int expected = int(grid->getAllVoxelData()->density->getCurrentValue(Vector3(targetx, targety, targetz)));

	EXPECT_EQ(targetValue, expected) << "Setting Density at a point 2D does not set properly!";
}

TEST(CFDGrid, setDensity2D) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int size = 5;

	int targetx = 2;
	int targety = 2;
	int targetz = 0;

	int targetValue = 204;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();
	grid->setGrid(size, 2);

	grid->getAllVoxelData()->density->setCurrentValue(Vector3(targetx, targety, targetz), targetValue);
	int expected = int(grid->getAllVoxelData()->density->getCurrentValue(Vector3(targetx, targety, targetz)));

	EXPECT_EQ(targetValue, expected) << "Setting Density at a point 2D does not set properly!";
}

TEST(CFDGrid, setDensity3D) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int size = 5;

	int targetx = 2;
	int targety = 2;
	int targetz = 2;

	int targetValue = 204;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();
	grid->setGrid(size, 3);

	grid->getAllVoxelData()->density->setCurrentValue(Vector3(targetx, targety, targetz), targetValue);
	int expected = int(grid->getAllVoxelData()->density->getCurrentValue(Vector3(targetx, targety, targetz)));

	EXPECT_EQ(targetValue, expected) << "Setting Density at a point 3D does not set properly!";
}

TEST(CFDGrid, getInvalidIndex2D) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int size = 5;

	int targetx = 10;
	int targety = 10;
	int targetz = 0;

	int targetValue = 204;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();
	grid->setGrid(size, 2);

	int expected = int(grid->getAllVoxelData()->density->getCurrentValue(Vector3(targetx, targety, targetz)));

	EXPECT_EQ(expected, 0) << "Getting a invalid point doesnt return properly!";
}

TEST(CFDGrid, getInvalidIndex3D) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int size = 5;

	int targetx = 10;
	int targety = 10;
	int targetz = 10;

	int targetValue = 204;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();
	grid->setGrid(size, 3);

	int expected = int(grid->getAllVoxelData()->density->getCurrentValue(Vector3(targetx, targety, targetz)));

	EXPECT_EQ(expected, 0) << "Getting a invalid point doesnt return properly!";
}

TEST(CFDSim, emptySimulation) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int size = 5;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();
	grid->setGrid(size, 2);
	grid->Start();

	grid->setLogging(false);

	for(int i = 0; i < 50; ++i)
	{
		grid->Update(0.016f);
	}

	std::vector<int> failedIndexes;
	for(int i = 0; i < grid->getGridSize(); ++i)
	{
		if(grid->getAllVoxelData()->density->getCurrentValue(i) == 0)
		{
			failedIndexes.push_back(i);
		}
	}

	EXPECT_FALSE(failedIndexes.size() == 0) << "Some indexes where not zero when playing the simulation with no forces. Failed Indexes Count: " << failedIndexes.size();
}

TEST(CFDSim, diffusion2D) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int size = 5;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();
	grid->setGrid(size, 2);
	grid->Start();

	grid->setLogging(false);

	const Vector3 target = Vector3(0, 0, 0);

	for (int i = 0; i < 50; ++i)
	{
		grid->addDensity(target, 10);
		grid->Update(0.016f);
	}

	const float expected = 0.96635f;
	float value = grid->getAllVoxelData()->density->getCurrentValue(target);

	EXPECT_TRUE(Math::compareFloat(expected, value, 0.00001f)) << "Expected: " << expected << " Value: " << value;
}

TEST(CFDSim, diffusion3D) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int size = 5;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();
	grid->setGrid(size, 3);
	grid->Start();

	grid->setLogging(false);

	const Vector3 target = Vector3(0, 0, 0);

	for (int i = 0; i < 50; ++i)
	{
		grid->addDensity(target, 10);
		grid->Update(0.016f);
	}

	const float expected = 0.20130f;
	float value = grid->getAllVoxelData()->density->getCurrentValue(target);

	EXPECT_TRUE(Math::compareFloat(expected, value, 0.00001f)) << "Expected: " << expected << " Value: " << value;
}

TEST(CFDSim, velocity2D) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int size = 5;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();
	grid->setGrid(size, 2);
	grid->setViscocity(1.0f);
	grid->Start();

	grid->setLogging(false);

	const Vector3 target = Vector3(1, 0, 0);
	const Vector3 velo = Vector3(100,100,100);

	for (int i = 0; i < 5; ++i)
	{
		grid->addVelocity(target, velo);
		grid->Update(0.016f);
	}

	const float expectedX = -2.12018f;
	const float expectedY = 5.822224f;
	const float expectedZ = 0.0f;

	float valueX = grid->getAllVoxelData()->velocityX->getCurrentValue(target);
	float valueY = grid->getAllVoxelData()->velocityY->getCurrentValue(target);
	float valueZ = grid->getAllVoxelData()->velocityZ->getCurrentValue(target);

	bool value = (Math::compareFloat(expectedX, valueX, 0.00001f) &&
				  Math::compareFloat(expectedY, valueY, 0.00001f) &&
				  Math::compareFloat(expectedZ, valueZ, 0.00001f));

	EXPECT_TRUE(value) << "Expected: (" << expectedX << "," << expectedY << "," << expectedZ << ") Value: ("<< valueX << "," << valueY << "," << valueZ << ")";
}

TEST(CFDSim, velocity3D) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

	int size = 5;

	CFD::CFDGrid* grid = object.addComponent<CFD::CFDGrid>();
	grid->setGrid(size, 3);
	grid->setViscocity(1.0f);
	grid->Start();

	grid->setLogging(false);

	const Vector3 target = Vector3(1, 0, 0);
	const Vector3 velo = Vector3(100, 100, 100);

	for (int i = 0; i < 5; ++i)
	{
		grid->addVelocity(target, velo);
		grid->Update(0.016f);
	}

	const float expectedX = -0.33716f;
	const float expectedY = 0.0f;
	const float expectedZ = 0.0f;

	float valueX = grid->getAllVoxelData()->velocityX->getCurrentValue(target);
	float valueY = grid->getAllVoxelData()->velocityY->getCurrentValue(target);
	float valueZ = grid->getAllVoxelData()->velocityZ->getCurrentValue(target);

	bool value = (Math::compareFloat(expectedX, valueX, 0.00001f) &&
		Math::compareFloat(expectedY, valueY, 0.00001f) &&
		Math::compareFloat(expectedZ, valueZ, 0.00001f));

	EXPECT_TRUE(value) << "Expected: (" << expectedX << "," << expectedY << "," << expectedZ << ") Value: (" << valueX << "," << valueY << "," << valueZ << ")";
}