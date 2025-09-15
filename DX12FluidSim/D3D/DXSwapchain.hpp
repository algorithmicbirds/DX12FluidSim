#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>
#include <stdexcept>
#include <iostream>

#include "Shared/Utils.hpp"

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
    inline D3D12_CPU_DESCRIPTOR_HANDLE &GetCurrentRTVHandle()
    {
        return RTVHandles.at(SwapChain3->GetCurrentBackBufferIndex());
    }
    inline D3D12_CPU_DESCRIPTOR_HANDLE &GetCurrentDSVHandle() { return DSVHandle; }
    inline UINT GetHeight() const { return Height; }
    inline UINT GetWidth() const { return Width; }
    inline void TransitionCurrentToRT(ID3D12GraphicsCommandList7 *CmdList)
    {
        Utils::TransitionResoure(
            CmdList, GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET
        );
    }
    inline void TransitionRTToPresent(ID3D12GraphicsCommandList7 *CmdList)
    {
        Utils::TransitionResoure(
            CmdList, GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT
        );
    }

    void ResolveToBackBuffers(ID3D12GraphicsCommandList7* CmdList);
    D3D12_CPU_DESCRIPTOR_HANDLE &GetMSAARTVHandle() { return MSAARTVHandle; }
    D3D12_CPU_DESCRIPTOR_HANDLE &GetMSAADSVHandle() { return MSAADSVHandle; }
    ID3D12Resource2 *GetMSAARTVResource() { return MSAARTV.Get(); }

private:
    bool Init();
    bool GetBuffers();
    void ReleaseBuffers();
    void UpdateViewportAndScissor();
    void CreateRTVAndDescHeap();
    void ReleaseRTVHeaps();
    void ReleaseMSAARTVHeap();
    void CreateDSV();
    void CreateMSAADSVAndBuffer();
    void CreateDepthStencilBuffer();
    void CreateMSAARTVAndDescHeap();
    void ReleaseDSV();
    void MSAAReleaseDSV();
    void InitializeViews();

private:
    DXContext &ContextRef;
    static constexpr size_t FrameCount = 3;
    ComPtr<IDXGISwapChain3> SwapChain3;
    ComPtr<IDXGIFactory7> DXGIFactory;
    ComPtr<ID3D12Resource2> DepthBuffer;
    ComPtr<ID3D12DescriptorHeap> RTVDescHeap;
    ComPtr<ID3D12DescriptorHeap> MSAARTVDescHeap;
    ComPtr<ID3D12Resource2> Buffers[FrameCount];
    ComPtr<ID3D12Resource1> DepthStencilBuffer;
    ComPtr<ID3D12Resource1> MSAADepthStencilBuffer;
    ComPtr<ID3D12Resource2> MSAARTV;
    ComPtr<ID3D12DescriptorHeap> DSVHeap;
    ComPtr<ID3D12DescriptorHeap> MSAADSVHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE DSVHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE MSAARTVHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE MSAADSVHandle;

    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RTVHandles;
    HWND HwndRef;
    UINT Height = 1080;
    UINT Width = 1920;

    D3D12_VIEWPORT Viewport{};
    D3D12_RECT ScissorRect{};
};