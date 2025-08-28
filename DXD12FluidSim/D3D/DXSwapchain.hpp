#pragma once

#include "GlobInclude/WinInclude.hpp"

class DXContext;
class Window;

class DXSwapChain
{
public:
    DXSwapChain(DXContext &Context, HWND WindowHandle);
    ~DXSwapChain();
    DXSwapChain &operator=(const DXSwapChain &) = delete;
    DXSwapChain(const DXSwapChain &) = delete;


public:
    void Present();
    static constexpr size_t GetFrameCount() { return 3; }
    void Resize();

private:
    bool Init();
    void ShutDown();

private:
    DXContext &ContextRef;
    ComPtr<IDXGISwapChain3> SwapChain3;
    HWND HwndRef;
    UINT Height = 1080;
    UINT Width = 1920;
};