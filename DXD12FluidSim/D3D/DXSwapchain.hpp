#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>

class DXContext;
class Window;

class DXSwapchain
{
public:
    DXSwapchain(DXContext &Context, HWND WindowHandle);
    ~DXSwapchain();
    DXSwapchain &operator=(const DXSwapchain &) = delete;
    DXSwapchain(const DXSwapchain &) = delete;


public:
    void Present();
    static constexpr size_t GetFrameCount() { return FrameCount; }
    void Resize();
    inline ID3D12Resource1 *GetCurrentBackBuffer() const
    {
        UINT index = SwapChain3->GetCurrentBackBufferIndex();
        return Buffers[index].Get();
    }

    inline UINT GetCurrentBackBufferIndex() const { return CurrentBackBufferIndex; }


private:
    bool Init();
    void ShutDown();
    bool GetBuffers();
    void ReleaseBuffers();


private:
    DXContext &ContextRef;
    static constexpr size_t FrameCount = 3;
    ComPtr<IDXGISwapChain3> SwapChain3;
    ComPtr<ID3D12Resource1> Buffers[FrameCount];
    UINT CurrentBackBufferIndex;
    HWND HwndRef;
    UINT Height = 1080;
    UINT Width = 1920;
};