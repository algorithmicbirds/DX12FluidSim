#include "Window/Window.hpp"
#include <iostream>


bool Window::bShouldClose = false;

Window::Window() { Init(); }

Window::~Window() { ShutDown(); }

bool Window::Init()
{
    WNDCLASSEXW wcexw{};
    wcexw.cbSize = sizeof(wcexw);
    wcexw.style = CS_OWNDC;
    wcexw.lpfnWndProc = &Window::OnWindowMessage;
    wcexw.cbClsExtra = 0;
    wcexw.cbWndExtra = 0;
    wcexw.hInstance = GetModuleHandle(nullptr);
    wcexw.hIcon = LoadIconW(nullptr, MAKEINTRESOURCEW(IDI_APPLICATION));
    wcexw.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(IDC_ARROW));
    wcexw.hbrBackground = nullptr;
    wcexw.lpszMenuName = nullptr;
    wcexw.lpszClassName = L"D3D12ExWndClass";
    wcexw.hIconSm = LoadIconW(nullptr, MAKEINTRESOURCEW(IDI_APPLICATION));
    WindowClass = RegisterClassExW(&wcexw);
    if (WindowClass == 0)
    {
        return false;
    }

    //constexpr wchar_t WindowTitle[] = "DX12 FluidSim";

    Hwnd = CreateWindowExW(
        WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,
        wcexw.lpszClassName,
        L"DX12 Fluid Sim",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100,
        100,
        1280,
        720,
        nullptr,
        nullptr,
        wcexw.hInstance,
        nullptr
    );

    return true;
}

void Window::ShutDown()
{
    if (Hwnd)
    {
        DestroyWindow(Hwnd);
    }
    if (WindowClass)
    {
        UnregisterClassW(L"D3D12ExWndClass", GetModuleHandleW(nullptr));
    }
}

void Window::Update()
{
    MSG msg;
    while (PeekMessageW(&msg, Hwnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

LRESULT Window::OnWindowMessage(HWND Window, UINT MSG, WPARAM WParam, LPARAM LParam)
{
    switch (MSG)
    {
    case WM_CLOSE:
        Window::bShouldClose = true;
        break;
    default:
        break;
    }
    return DefWindowProc(Window, MSG, WParam, LParam);
}
