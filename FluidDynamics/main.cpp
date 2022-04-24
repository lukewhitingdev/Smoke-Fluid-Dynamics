//--------------------------------------------------------------------------------------
// File: main.cpp
//
// This application demonstrates animation using matrix transformations
//
// http://msdn.microsoft.com/en-us/library/windows/apps/ff729722.aspx
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#define _XM_NO_INTRINSICS_

#include "main.h"
#include "Core/Entity System/Entity.h"
#include "Core/Components/Test/TestComponent.h"
#include <Utility/Shader/ShaderUtility.h>
#include <Utility/Time/Time.h>
#include <Utility/Window/Headers/Window.h>
#include <Utility/Direct3D/Headers/D3D.h>
#include "Core/Components/Transform/Transform.h"
#include <Core/Components/Camera/Camera.h>
#include <Core/Components/Grid/Grid.h>
#include <Core/Components/LineMesh/LineMesh.h>
#include <Core/Components/CFD/Grid/CFDGrid.h>

#include "Dependencies\UI\IMGUI\imgui.h"
#include "Dependencies\UI\IMGUI\imgui_impl_dx11.h"
#include "Dependencies\UI\IMGUI\imgui_impl_win32.h"

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT	InitMesh();
HRESULT	InitWorld(int width, int height);
void InitUI();
void RenderUI();
void CleanupDevice();
void Update();
void Render();



//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
ID3D11Buffer*           matrixConstBuffer = nullptr;
ID3D11Buffer*           g_pLightConstantBuffer = nullptr;

GameObject*		gameObject;
std::vector<GameObject*> gameObjects;

Window* window = Window::getInstance();
D3D* direct3D = D3D::getInstance();


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if (FAILED(window->InitWindow(hInstance, nCmdShow)))
    {
        MessageBox(nullptr,
            L"Window could not be initialized.", L"Error", MB_OK);
        return 0;
    };

    if (FAILED(direct3D->InitDevice()))
    {
        MessageBox(nullptr,
            L"Device could not be initialized.", L"Error", MB_OK);
        CleanupDevice();
        return 0;
    }

    InitUI();

    InitMesh();

    InitWorld(1280, 720);

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            TimeUtility::calculateDeltaTime();
            if(!direct3D->userResizing)
            {
                Update();
                Render();
            }
        }
    }

    CleanupDevice();

    return ( int )msg.wParam;
}


// ***************************************************************************************
// InitMesh
// ***************************************************************************************

HRESULT		InitMesh()
{
	HRESULT hr;

	// Create the constant buffer
    D3D11_BUFFER_DESC bd = {};
    bd.CPUAccessFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MatrixBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = direct3D->device->CreateBuffer(&bd, nullptr, &matrixConstBuffer);
	if (FAILED(hr))
		return hr;



	// Create the light constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(LightsConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = direct3D->device->CreateBuffer(&bd, nullptr, &g_pLightConstantBuffer);
	if (FAILED(hr))
		return hr;


	return hr;
}

GameObject* camera;
GameObject* grid;
CFD::CFDGrid* cfd;
Grid* gridComponent;
Camera* cam;

// ***************************************************************************************
// InitWorld
// ***************************************************************************************
HRESULT		InitWorld(int width, int height)
{
    //gameObjects.emplace_back(new GameObject("Test1"));
    //gameObjects.emplace_back(new GameObject("Test2"));
    //gameObjects[0]->setPosition(DirectX::XMFLOAT3(0, 0, 0));
    //gameObjects[0]->setPosition(DirectX::XMFLOAT3(-5, 0, 0));

    //FILE* fpstdout = stdout;

    //AllocConsole();
    //freopen_s(&fpstdout, "CONOUT$", "w", stdout);
    //printf("Debugging Window:\n");

    camera = new GameObject("Camera");
    camera->setRenderable(false);
    camera->setUpdateable(true);
    cam = camera->addComponent<Camera>();
    cam->setTransfrom(camera->getTransform());
    camera->setPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -3.0f));
    cam->updateProjection(width, height);
    camera->removeMaterial();
    camera->removeMesh();
    gameObjects.emplace_back(camera);

    grid = new GameObject("Grid");
    grid->removeMesh();
    grid->removeMaterial();
    gridComponent = grid->addComponent<Grid>();
    CFD::CFDGrid* CFD = grid->addComponent<CFD::CFDGrid>();
    cfd = CFD;
   
    gridComponent->setMatrices(grid->getTransform()->getWorld(), cam->getViewMatrix(), cam->getProjectionMatrix());

    grid->getTransform()->setPosition(DirectX::XMFLOAT3(0, 0, 0));
    gameObjects.emplace_back(grid);

	return S_OK;
}

void InitUI()
{
    // Init IMGUI
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(window->windowHandle);
    ImGui_ImplDX11_Init(direct3D->device, direct3D->immediateContext);
}

void RenderUI()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Draw Stuff.
    static int selectedPosX;
    static int selectedPosY;
    static int selectedPosZ;
    static bool editingVoxel;

    ImGui::Begin("Voxel Controls");
    ImGui::SliderInt("Select Voxel X", &selectedPosX, 0, cfd->getGridWidth());
    ImGui::SliderInt("Select Voxel Y", &selectedPosY, 0, cfd->getGridHeight());
    ImGui::SliderInt("Select Voxel Z", &selectedPosZ, 0, cfd->getGridHeight());

    CFD::CFDVoxel vox = cfd->getVoxel(Vector3(selectedPosX, selectedPosY, selectedPosZ));
    gridComponent->setSelectedGridItem(Vector3(selectedPosX, selectedPosY, selectedPosZ));

    if (editingVoxel == false)
    {
        // Non editing mode.

        ImGui::Text("Voxel Data:");
        ImGui::Text("Position: %d, %d, %d", vox.position.x, vox.position.y, vox.position.z);
        ImGui::Text("Density: %f", vox.density);
        ImGui::Text("Velocity: %f, %f, %f", vox.velocity.x, vox.velocity.y, vox.velocity.z);

        if (ImGui::Button("Edit"))
        {
            editingVoxel = true;
        }
    }
    else
    {
        // Editing mode.

        static float veloEdit[3];
        static float editedDens;

        ImGui::Text("Edit Voxel Data:");
        ImGui::SliderFloat("Density", &editedDens, 0, 1000);
        ImGui::SliderFloat3("Velocity", veloEdit, 0, 150);

        if(ImGui::Button("Add"))
        {
            cfd->addVelocity(vox.position, Vector3(veloEdit[0], veloEdit[1], veloEdit[2]));
            cfd->addDensity(vox.position, editedDens);
        }

        if (ImGui::Button("Back"))
        {
            editingVoxel = false;
        }
    }



    ImGui::End();

    static int domainSize = cfd->getGridSize();
    static float diffusionRate = cfd->getDiffusionRate();
    static float viscocityRate = cfd->getViscocity();
    static int veloMinMax;

    ImGui::Begin("Domain Controls");
    ImGui::InputInt("Size", &domainSize);

    ImGui::SliderFloat("Diffusion Rate", &diffusionRate, 0, 1);
    cfd->setDiffusionRate(diffusionRate);

    ImGui::SliderFloat("Viscocity Rate", &viscocityRate, 0, 1);
    cfd->setViscocity(viscocityRate);

    ImGui::SliderInt("Random Velocity MinMax", &veloMinMax, 0, 10);
    cfd->setRandomVelocityMinMax(veloMinMax);

    ImGui::Separator();

    if (ImGui::Button("Save"))
    {
        int dimensions = 2;
        if (dimensions == 3)
            gridComponent->GenerateGrid(domainSize, domainSize, domainSize);
        else
            gridComponent->GenerateGrid(domainSize, domainSize, 1);

        cfd->setGrid(domainSize, dimensions);
        cfd->Start();
    };


    ImGui::End();

    ImGui::Begin("Stats");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    gameObject->cleanup();

    // Remove any bound render target or depth/stencil buffer
    ID3D11RenderTargetView* nullViews[] = { nullptr };
    direct3D->immediateContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);

    if(direct3D->immediateContext) direct3D->immediateContext->ClearState();
    direct3D->immediateContext->Flush();

    if (g_pLightConstantBuffer)
        g_pLightConstantBuffer->Release();
    if( matrixConstBuffer ) matrixConstBuffer->Release();
    if(direct3D->immediateContext) direct3D->immediateContext->Release();


    ID3D11Debug* debugDevice = nullptr;
    direct3D->device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugDevice));

    if (direct3D->device) direct3D->device->Release();

    if(debugDevice)
    {
        // handy for finding dx memory leaks
        debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        debugDevice->Release();
    }
}


void setupLightForRender()
{
    Light light;
    light.Enabled = static_cast<int>(true);
    light.LightType = PointLight;
    light.Color = XMFLOAT4(Colors::White);
    light.SpotAngle = XMConvertToRadians(45.0f);
    light.ConstantAttenuation = 1.0f;
    light.LinearAttenuation = 1;
    light.QuadraticAttenuation = 1;


    // set up the light
    XMFLOAT4 LightPosition(XMFLOAT4(0.0f, 0, -3, 1.0f));
    light.Position = LightPosition;
    XMVECTOR LightDirection = XMVectorSet(-LightPosition.x, -LightPosition.y, -LightPosition.z, 0.0f);
    LightDirection = XMVector3Normalize(LightDirection);
    XMStoreFloat4(&light.Direction, LightDirection);

    LightsConstantBuffer lightProperties;
    lightProperties.EyePosition = LightPosition;
    lightProperties.Lights[0] = light;
    direct3D->immediateContext->UpdateSubresource(g_pLightConstantBuffer, 0, nullptr, &lightProperties, 0, 0);
}

MatrixBuffer cb;
void Update()
{
    float t = TimeUtility::getDeltaTime(); // capped at 60 fps
    if (t == 0.0f)
        return;

    for (int i = 0; i < gameObjects.size(); i++)
    {
        // Update the cube transform, material etc. 
        gameObjects[i]->update(t);
    }
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
    // Clear the back buffer
    direct3D->immediateContext->ClearRenderTargetView( direct3D->renderTargetView, Colors::Red );

    // Clear the depth buffer to 1.0 (max depth)
    direct3D->immediateContext->ClearDepthStencilView(direct3D->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

    setupLightForRender();

    Camera* camComponent = camera->getComponent<Camera>();

    XMFLOAT4X4* floatView = camComponent->getViewMatrix();
    XMFLOAT4X4* floatProj = camComponent->getProjectionMatrix();

    // Render the cube
    direct3D->immediateContext->VSSetConstantBuffers(0, 1, &matrixConstBuffer);
    direct3D->immediateContext->PSSetConstantBuffers(2, 1, &g_pLightConstantBuffer);

    for(int i = 0; i < gameObjects.size(); i++)
    {
        // get the game object world transform
        XMMATRIX mGO = XMLoadFloat4x4(gameObjects[i]->getWorld());

        cb.mWorld = XMMatrixTranspose(mGO);
        cb.mView = XMMatrixTranspose(XMLoadFloat4x4(floatView));
        cb.mProjection = XMMatrixTranspose(XMLoadFloat4x4(floatProj));
        direct3D->immediateContext->UpdateSubresource(matrixConstBuffer, 0, nullptr, &cb, 0, 0);

        gameObjects[i]->draw();
    }

    RenderUI();

    // Present our back buffer to our front buffer
    direct3D->swapChain->Present( 0, 0 );
}



