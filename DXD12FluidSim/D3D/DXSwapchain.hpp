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

    inline ID3D12Resource1 *GetBuffer(UINT index) const
    {
        if (index >= FrameCount)
            return nullptr;
        return Buffers[index].Get();
    }

    inline const D3D12_VIEWPORT &GetViewport() const { return Viewport; }
    inline const D3D12_RECT &GetScissorRect() const { return ScissorRect; }
    inline UINT GetCurrentBackBufferIndex() const { return SwapChain3->GetCurrentBackBufferIndex(); }
    inline float GetAspectRatio() const { return static_cast<float>(Width) / static_cast<float>(Height); }

private:
    bool Init();
    void ShutDown();
    bool GetBuffers();
    void ReleaseBuffers();
    void UpdateViewportAndScissor();

private:
    DXContext &ContextRef;
    static constexpr size_t FrameCount = 3;
    ComPtr<IDXGISwapChain3> SwapChain3;
    ComPtr<IDXGIFactory7> DXGIFactory;
    ComPtr<ID3D12Resource1> Buffers[FrameCount];
    UINT CurrentBackBufferIndex;
    HWND HwndRef;
    UINT Height = 1080;
    UINT Width = 1920;

    D3D12_VIEWPORT Viewport{};
    D3D12_RECT ScissorRect{};
};