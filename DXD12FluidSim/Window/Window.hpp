#pragma once
#include "GlobInclude/WinInclude.hpp"

class Window
{
public:
    Window();
    ~Window();
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    void Update();

    bool ShouldClose() const { return bShouldClose; }
    bool ShouldResize() const { return bShouldResize; }
    void ClearResizeFlags() { bShouldResize = false; }

    HWND GetHwnd() const { return Hwnd; }
    void SetFullScreen(bool EnableFullScreen);
    void FullScreenFlipFlop();

private:
    bool Init();
    void ShutDown();
    static LRESULT CALLBACK StaticWindowProc(HWND Hwnd, UINT Msg, WPARAM WParam, LPARAM LParam);
    LRESULT WindowProc(HWND Hwnd, UINT Msg, WPARAM WParam, LPARAM LParam);

private:
    HWND Hwnd = nullptr;
    ATOM WindowClass = 0;
    RECT SavedWindowRect{};

    static inline bool bShouldClose = false;
    static inline bool bShouldResize = false;
    static inline bool bIsFullScreenEnabled = false;

    DWORD DefaultStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    DWORD EXDefaultStyle = WS_EX_APPWINDOW;
};
