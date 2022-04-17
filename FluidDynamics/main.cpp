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

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT	InitMesh();
HRESULT	InitWorld(int width, int height);
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

// ***************************************************************************************
// InitWorld
// ***************************************************************************************
HRESULT		InitWorld(int width, int height)
{
    //gameObjects.emplace_back(new GameObject("Test1"));
    //gameObjects.emplace_back(new GameObject("Test2"));
    //gameObjects[0]->setPosition(DirectX::XMFLOAT3(0, 0, 0));
    //gameObjects[0]->setPosition(DirectX::XMFLOAT3(-5, 0, 0));

    FILE* fpstdout = stdout;

    AllocConsole();
    freopen_s(&fpstdout, "CONOUT$", "w", stdout);
    printf("Debugging Window:\n");

    camera = new GameObject("Camera");
    camera->setRenderable(false);
    camera->setUpdateable(true);
    Camera* cam = camera->addComponent<Camera>();
    cam->setTransfrom(camera->getTransform());
    camera->setPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -3.0f));
    cam->updateProjection(width, height);
    camera->removeMaterial();
    camera->removeMesh();
    gameObjects.emplace_back(camera);

    grid = new GameObject("Grid");
    grid->removeMesh();
    grid->removeMaterial();
    Grid* gridComponent = grid->addComponent<Grid>();
    CFD::CFDGrid* CFD = grid->addComponent<CFD::CFDGrid>();
    cfd = CFD;
    
    int w = 4;
    int h = 4;
    int d = 1;

    gridComponent->setMatrices(grid->getTransform()->getWorld(), cam->getViewMatrix(), cam->getProjectionMatrix());
    gridComponent->GenerateGrid(w, h, d);

    CFD->setGrid(w, h, d);
    cfd->addVelocitySource(Vector3(2, 3, 0), Vector3(0.0f, 10.0f, 0.0f));
    CFD->Start();

    grid->getTransform()->setPosition(DirectX::XMFLOAT3(0, 0, 0));
    gameObjects.emplace_back(grid);

	return S_OK;
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

    cfd->addDensitySource(0, 0, 0, 10.0f);


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
    direct3D->immediateContext->ClearRenderTargetView( direct3D->renderTargetView, Colors::MidnightBlue );

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

    // Present our back buffer to our front buffer
    direct3D->swapChain->Present( 0, 0 );
}



