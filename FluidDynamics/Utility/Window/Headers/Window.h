#pragma once
#include <d3d11_1.h>
#include <windowsx.h>

class Window
{

#pragma region Singleton

private:
	Window();

public:
	static Window* getInstance();

#pragma endregion

private:
	HINSTANCE instanceHandle;

public:
	int viewWidth;
	int viewHeight;
	HWND windowHandle;
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);

};

