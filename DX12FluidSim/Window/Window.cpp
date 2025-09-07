#include "Window.hpp"
#include <stdexcept>

Window::Window()
{
    if (!Init())
    {
        throw std::runtime_error("Failed to create Window");
    }

    QueryPerformanceCounter(&Frequency);
    QueryPerformanceCounter(&LastTime);
}

Window::~Window() { ShutDown(); }

bool Window::Init()
{
    WNDCLASSEXW wcexw{};
    wcexw.cbSize = sizeof(wcexw);
    wcexw.style = CS_OWNDC;
    wcexw.lpfnWndProc = &Window::StaticWindowProc;
    wcexw.hInstance = GetModuleHandleW(nullptr);
    wcexw.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    wcexw.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wcexw.lpszClassName = L"D3D12ExWndClass";

    WindowClass = RegisterClassExW(&wcexw);
    if (WindowClass == 0)
        return false;

    Hwnd = CreateWindowExW(
        EXDefaultStyle,
        wcexw.lpszClassName,
        L"DX12 Fluid Sim",
        DefaultStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1280,
        720,
        nullptr,
        nullptr,
        wcexw.hInstance,
        this
    );

    if (!Hwnd)
        return false;

    // Save the rect for fullscreen restore
    GetWindowRect(Hwnd, &SavedWindowRect);

    return true;
}

void Window::ShutDown()
{
    if (Hwnd)
    {
        DestroyWindow(Hwnd);
        Hwnd = nullptr;
    }
    if (WindowClass)
    {
        UnregisterClassW(L"D3D12ExWndClass", GetModuleHandleW(nullptr));
        WindowClass = 0;
    }
}

void Window::Update()
{
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void Window::FullScreenFlipFlop() { SetFullScreen(!bIsFullScreenEnabled); }

float Window::GetDeltaTimeSeconds()
{
    LARGE_INTEGER CurrentTime;
    QueryPerformanceCounter(&CurrentTime);

    float deltaTime = static_cast<float>(CurrentTime.QuadPart - LastTime.QuadPart) / Frequency.QuadPart;

    LastTime = CurrentTime;
    return deltaTime;
}

void Window::SetFullScreen(bool EnableFullScreen)
{
    if (EnableFullScreen == bIsFullScreenEnabled)
        return;

    if (EnableFullScreen)
    {
        GetWindowRect(Hwnd, &SavedWindowRect);

        SetWindowLongW(Hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
        SetWindowLongW(Hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW);

        MONITORINFO mi = {sizeof(mi)};
        if (GetMonitorInfoW(MonitorFromWindow(Hwnd, MONITOR_DEFAULTTONEAREST), &mi))
        {
            SetWindowPos(
                Hwnd,
                HWND_TOP,
                mi.rcMonitor.left,
                mi.rcMonitor.top,
                mi.rcMonitor.right - mi.rcMonitor.left,
                mi.rcMonitor.bottom - mi.rcMonitor.top,
                SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOACTIVATE
            );
        }

        bIsFullScreenEnabled = true;
    }
    else
    {
        SetWindowLongW(Hwnd, GWL_STYLE, DefaultStyle);
        SetWindowLongW(Hwnd, GWL_EXSTYLE, EXDefaultStyle);

        SetWindowPos(
            Hwnd,
            HWND_NOTOPMOST,
            SavedWindowRect.left,
            SavedWindowRect.top,
            SavedWindowRect.right - SavedWindowRect.left,
            SavedWindowRect.bottom - SavedWindowRect.top,
            SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOACTIVATE
        );

        bIsFullScreenEnabled = false;
    }
}



LRESULT CALLBACK Window::StaticWindowProc(HWND Hwnd, UINT Msg, WPARAM WParam, LPARAM LParam)
{
    Window *self = nullptr;

    if (Msg == WM_NCCREATE)
    {
        CREATESTRUCTW *cs = reinterpret_cast<CREATESTRUCTW *>(LParam);
        self = reinterpret_cast<Window *>(cs->lpCreateParams);
        SetWindowLongPtrW(Hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    }
    else
    {
        self = reinterpret_cast<Window *>(GetWindowLongPtrW(Hwnd, GWLP_USERDATA));
    }

    if (self)
        return self->WindowProc(Hwnd, Msg, WParam, LParam);

    return DefWindowProcW(Hwnd, Msg, WParam, LParam);
}

LRESULT Window::WindowProc(HWND Hwnd, UINT Msg, WPARAM WParam, LPARAM LParam)
{
    switch (Msg)
    {
    case WM_KEYDOWN:
        if (WParam == VK_F11)
            FullScreenFlipFlop();
        break;

    case WM_CLOSE:
        bShouldClose = true;
        break;

    case WM_SIZE:
        if (WParam != SIZE_MINIMIZED)
            bShouldResize = true;
        break;
    default:
        break;
    }
    return DefWindowProcW(Hwnd, Msg, WParam, LParam);
}
