#pragma once

#include "GlobInclude/WinInclude.hpp"

class Window
{
public:
    Window();
    ~Window();
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    ATOM WindowClass = 0;
    void Update();

    inline bool ShouldClose() { return bShouldClose; }
    inline bool ShouldResize() { return bShouldResize; }
    void ClearResizeFlags();
    HWND GetHwnd() const { return Hwnd; }

private:
    bool Init();
    void ShutDown();
    static LRESULT CALLBACK OnWindowMessage(HWND Hwnd, UINT MSG, WPARAM WParam, LPARAM LParam);

private:
    HWND Hwnd;
    static bool bShouldClose;
    static bool bShouldResize;
};