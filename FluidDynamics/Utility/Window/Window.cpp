#include "Utility/Window/Headers/Window.h"
#include "Utility/Direct3D/Headers/D3D.h"
#include "Utility/Input System/InputSystem.h"

#include "Dependencies\UI\IMGUI\imgui.h"
#include "Dependencies\UI\IMGUI/imgui_impl_dx11.h"
#include "Dependencies\UI\IMGUI/imgui_impl_win32.h"


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    static bool lockCursor;

    PAINTSTRUCT ps;
    HDC hdc;


    switch (message)
    {
        case WM_RBUTTONDOWN:
            lockCursor = true;
            break;

        case WM_RBUTTONUP:
            lockCursor = false;
            break;


        case WM_MOUSEMOVE:
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            if(lockCursor)
            {
                InputSystem::setMousePosition(xPos, yPos);
            }

            break;
        }
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_SIZE:

            switch (wParam) 
            {
                case SIZE_MAXIMIZED:
                    D3D::getInstance()->OnResize();
                    break;

                case SIZE_RESTORED:
                    D3D::getInstance()->OnResize();
                    break;

                default:
                    break;
            }
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Window::Window() : instanceHandle(nullptr), windowHandle(nullptr),viewHeight(0), viewWidth(0){}

Window* Window::getInstance()
{
    static Window* instance;
    if (instance == nullptr) {
        instance = new Window();
    }
    return instance;
}


// Initializes the window.
HRESULT Window::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"WindowClass";
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    instanceHandle = hInstance;
    RECT rc = { 0, 0, 1280, 720 };

    viewWidth = 1280;
    viewHeight = 720;

    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    windowHandle = CreateWindow(L"WindowClass", L"Workshop Engine",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
        nullptr);
    if (!windowHandle)
        return E_FAIL;

    ShowWindow(windowHandle, nCmdShow);

    return S_OK;
}
