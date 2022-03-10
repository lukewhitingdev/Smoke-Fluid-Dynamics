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

/*
TODO:
Make this into a suite so we can re-use device and object to test other things,
just so that when running the tests we dont have to initDevice and create a gameObject everytime which is slow.
*/

TEST(CFDGrid, setGridSize) {

	D3D* device = D3D::getInstance();
	device->InitDevice();

	GameObject object = GameObject();

}