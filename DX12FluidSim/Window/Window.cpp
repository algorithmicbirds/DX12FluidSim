#include "Window.hpp"
#include "Icons/resource.h"
#include <iostream>
#include <stdexcept>

#include <imgui_impl_win32.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Window::Window()
{
    if (!Init())
    {
        throw std::runtime_error("Failed to create Window");
    }

    QueryPerformanceFrequency(&Frequency);
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
    wcexw.hIcon = static_cast<HICON>(
        LoadImageW(wcexw.hInstance, MAKEINTRESOURCEW(IDI_APPICON), IMAGE_ICON, 70, 70, LR_DEFAULTCOLOR)
    );
    wcexw.hIconSm = static_cast<HICON>(
        LoadImageW(wcexw.hInstance, MAKEINTRESOURCEW(IDI_APPICON), IMAGE_ICON, 70, 70, LR_DEFAULTCOLOR)
    );
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

void Window::UpdateMsg()
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

    float maxDelta = 0.05;
    if (deltaTime >= maxDelta)
        deltaTime = maxDelta;

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
    if (ImGui::GetCurrentContext())
    {
        ImGuiIO &io = ImGui::GetIO();
        if (ImGui_ImplWin32_WndProcHandler(Hwnd, Msg, WParam, LParam))
        {
            return 0;
        }
        if (io.WantCaptureMouse)
        {
            return DefWindowProcW(Hwnd, Msg, WParam, LParam);
        }
    }

    switch (Msg)
    {
    case WM_ENTERSIZEMOVE:
        bInModalLoop = true;
        break;
    case WM_EXITSIZEMOVE:
        bInModalLoop = false;
        break;
    case WM_KEYDOWN:
        if (!(LParam & 0x40000000))
        {
            Keyboard.OnKeyPressed(static_cast<UCHAR>(WParam));
        }
        break;
    case WM_KEYUP:
        Keyboard.OnKeyReleased(static_cast<UCHAR>(WParam));
        break;
    case WM_CLOSE:
        bShouldClose = true;
        break;
    case WM_CHAR:
        Keyboard.OnChar(static_cast<char>(WParam));
        break;
    case WM_MOUSEMOVE:
    {
        int x = GET_X_LPARAM(LParam);
        int y = GET_Y_LPARAM(LParam);
        RECT clientRect;
        GetClientRect(Hwnd, &clientRect);
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;
        bool bIsInside = x >= 0 && x < width && y >= 0 && y < height;
        Mouse.OnMouseMove(x, y);
        if (bIsInside && !Mouse.IsInWindow())
        {
            Mouse.OnMouseEnter();
        }
        else if (!bIsInside && Mouse.IsInWindow())
        {
            Mouse.OnMouseLeave();
        }
        break;
    }
    case WM_LBUTTONDOWN:
    {
        int x = GET_X_LPARAM(LParam);
        int y = GET_Y_LPARAM(LParam);
        Mouse.OnLeftPressed(x, y);
        break;
    }
    case WM_LBUTTONUP:
    {
        int x = GET_X_LPARAM(LParam);
        int y = GET_Y_LPARAM(LParam);
        Mouse.OnLeftReleased(x, y);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        int x = GET_X_LPARAM(LParam);
        int y = GET_Y_LPARAM(LParam);
        Mouse.OnRightPressed(x, y);
        break;
    }
    case WM_RBUTTONUP:
    {
        int x = GET_X_LPARAM(LParam);
        int y = GET_Y_LPARAM(LParam);
        Mouse.OnRightReleased(x, y);
        break;
    }
    case WM_MOUSEWHEEL:
    {
        int x = GET_X_LPARAM(LParam);
        int y = GET_Y_LPARAM(LParam);
        int delta = GET_WHEEL_DELTA_WPARAM(WParam);
        Mouse.OnWheelDelta(x, y, delta);
        break;
    }
    case WM_SIZE:
        if (WParam != SIZE_MINIMIZED)
            bShouldResize = true;
        break;
    default:
        break;
    }
    return DefWindowProcW(Hwnd, Msg, WParam, LParam);
}

void Window::UpdateKeyBoard()
{
    while (auto e = Keyboard.ReadKey())
    {
        if (e->IsPress())
        {
            if (!KeysState[e->GetCode()])
                KeyDown.set(e->GetCode());

            KeysState[e->GetCode()] = true;
        }
        else if (e->IsRelease())
        {
            KeyUp.set(e->GetCode());
            KeysState[e->GetCode()] = false;
        }
    }
}
