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

TEST(CFDGrid, getInvalidIndex) {

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