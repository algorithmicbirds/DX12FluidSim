#include "D3D/DXSwapchain.hpp"
#include "D3D/DXContext.hpp"

DXSwapChain::DXSwapChain(DXContext &Context, HWND Hwnd) : ContextRef(Context), HwndRef(Hwnd) { Init(); }

DXSwapChain::~DXSwapChain() {}


bool DXSwapChain::Init()
{
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

    IDXGIFactory7 *Factory = ContextRef.GetDXGIFactory();
    Factory->CreateSwapChainForHwnd(
        ContextRef.GetCommandQueue(), HwndRef, &SwapchainDesc, &FSwapChainDesc, nullptr, SwapChain1.GetAddressOf()
    );
    if (FAILED(SwapChain1->QueryInterface(IID_PPV_ARGS(&SwapChain3))))
    {
        return false;
    }

    return true;
}

void DXSwapChain::Present() { SwapChain3->Present(1, 0); }

void DXSwapChain::Flush(size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        ContextRef.SignalAndWait();
    }
}


void DXSwapChain::ShutDown() { SwapChain3.Reset(); }
