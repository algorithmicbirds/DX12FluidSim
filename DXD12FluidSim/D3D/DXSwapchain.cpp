#include "D3D/DXSwapchain.hpp"
#include "D3D/DXContext.hpp"
#include "Window/Window.hpp"
#include <stdexcept>
#include <string>
#include <iostream>

#include "DebugLayer/DebugMacros.hpp"

DXSwapchain::DXSwapchain(DXContext &Context, HWND Hwnd) : ContextRef(Context), HwndRef(Hwnd) { Init(); }

DXSwapchain::~DXSwapchain() {}

bool DXSwapchain::Init()
{
    DX_VALIDATE(CreateDXGIFactory2(0, IID_PPV_ARGS(&DXGIFactory)), DXGIFactory);

    DXGI_SWAP_CHAIN_DESC1 SwapchainDesc{};
    SwapchainDesc.Width = 1920;
    SwapchainDesc.Height = 1080;
    SwapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapchainDesc.Stereo = false;
    SwapchainDesc.SampleDesc.Count = 1;
    SwapchainDesc.SampleDesc.Quality = 0;
    SwapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapchainDesc.BufferCount = GetFrameCount();
    SwapchainDesc.Scaling = DXGI_SCALING_STRETCH;
    SwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    SwapchainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    SwapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC FSwapChainDesc{};
    FSwapChainDesc.Windowed = true;

    ComPtr<IDXGISwapChain1> SwapChain1;

    DX_VALIDATE(DXGIFactory->CreateSwapChainForHwnd(
        ContextRef.GetCommandQueue(), HwndRef, &SwapchainDesc, &FSwapChainDesc, nullptr, SwapChain1.GetAddressOf()
    ), SwapChain1);

    DX_VALIDATE(SwapChain1->QueryInterface(IID_PPV_ARGS(&SwapChain3)), SwapChain3);


    GetBuffers();

    return true;
}

void DXSwapchain::Present()
{
    SwapChain3->Present(0, 0);
}

void DXSwapchain::ShutDown()
{
    ReleaseBuffers();
    SwapChain3.Reset();
}

bool DXSwapchain::GetBuffers()
{
    for (size_t i = 0; i < FrameCount; ++i)
    {
        DX_VALIDATE(SwapChain3->GetBuffer(i, IID_PPV_ARGS(&Buffers[i])), Buffers[i]);
    }
    return true;
}

void DXSwapchain::ReleaseBuffers()
{
    for (size_t i = 0; i < FrameCount; ++i)
    {
        Buffers[i].Reset();
    }
}

void DXSwapchain::Resize()
{
    ReleaseBuffers();
    RECT cr;
    if (GetClientRect(HwndRef, &cr))
    {
        Height = cr.bottom - cr.top;
        Width = cr.right - cr.left;

        SwapChain3->ResizeBuffers(
            GetFrameCount(),
            Width,
            Height,
            DXGI_FORMAT_UNKNOWN,
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
        );
    }
    GetBuffers();
}
