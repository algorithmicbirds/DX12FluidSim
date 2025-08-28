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


private:
    bool Init();
    void ShutDown();
    HWND WindowHandle;
    static LRESULT CALLBACK OnWindowMessage(HWND Window, UINT MSG, WPARAM WParam, LPARAM LParam);

private:
    static bool bShouldClose;
    
};