#pragma once
#include "Utility/Window/Headers/Window.h"
class D3D
{
#pragma region Singleton

private:
	D3D();

public:
	static D3D* getInstance();

#pragma endregion

private:
	Window* window;

	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

public:
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* immediateContext = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11Texture2D* depthStencil = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
	D3D11_VIEWPORT viewport;

	//--------------------------------------------------------------------------------------
	// Create Direct3D device and swap chain
	//--------------------------------------------------------------------------------------
	HRESULT InitDevice();

	bool userResizing = false;
	// Resize from client rect.
	void OnResize();
};

