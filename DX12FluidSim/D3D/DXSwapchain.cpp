#include "D3D/DXSwapchain.hpp"
#include "D3D/DXContext.hpp"
#include "Window/Window.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
#include "Shared/RendererConfig.hpp"

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

    DX_VALIDATE(
        DXGIFactory->CreateSwapChainForHwnd(
            ContextRef.GetCommandQueue(), HwndRef, &SwapchainDesc, &FSwapChainDesc, nullptr, SwapChain1.GetAddressOf()
        ),
        SwapChain1
    );

    DX_VALIDATE(SwapChain1->QueryInterface(IID_PPV_ARGS(&SwapChain3)), SwapChain3);

    InitializeViews();
    return true;
}

void DXSwapchain::Present() { SwapChain3->Present(0, 0); }

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

void DXSwapchain::UpdateViewportAndScissor()
{
    Viewport = {0.0f, 0.0f, static_cast<float>(Width), static_cast<float>(Height), 0.0f, 1.0f};
    ScissorRect = {0, 0, static_cast<LONG>(Width), static_cast<LONG>(Height)};
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
    InitializeViews();
    UpdateViewportAndScissor();
}

void DXSwapchain::CreateRTVAndDescHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC DescHeapDesc;
    DescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    DescHeapDesc.NumDescriptors = GetFrameCount();
    DescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DescHeapDesc.NodeMask = 0;
    DX_VALIDATE(ContextRef.GetDevice()->CreateDescriptorHeap(&DescHeapDesc, IID_PPV_ARGS(&RTVDescHeap)), RTVDescHeap);

    RTVHandles.resize(FrameCount);
    D3D12_CPU_DESCRIPTOR_HANDLE FirstCPUDescHandle = RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
    UINT HandleIncrement = ContextRef.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    for (size_t i = 0; i < FrameCount; ++i)
    {
        RTVHandles.at(i) = FirstCPUDescHandle;
        RTVHandles.at(i).ptr += HandleIncrement * i;
    }
    for (size_t i = 0; i < FrameCount; ++i)
    {
        VALIDATE_PTR(RTVDescHeap);
        ID3D12Resource1 *buffer = GetBuffer(i);
        D3D12_RENDER_TARGET_VIEW_DESC RTV;
        RTV.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        RTV.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        RTV.Texture2D.MipSlice = 0;
        RTV.Texture2D.PlaneSlice = 0;

        ContextRef.GetDevice()->CreateRenderTargetView(buffer, &RTV, RTVHandles.at(i));
    }
}

void DXSwapchain::CreateMSAARTVAndDescHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC MSAAHeapDesc{};
    MSAAHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    MSAAHeapDesc.NumDescriptors = 1;
    MSAAHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    MSAAHeapDesc.NodeMask = 0;

    DX_VALIDATE(
        ContextRef.GetDevice()->CreateDescriptorHeap(&MSAAHeapDesc, IID_PPV_ARGS(&MSAARTVDescHeap)), MSAARTVDescHeap
    );

    MSAARTVHandle = MSAARTVDescHeap->GetCPUDescriptorHandleForHeapStart();

    // texture desc
    D3D12_RESOURCE_DESC MSAARTDesc{};
    MSAARTDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    MSAARTDesc.Height = Height;
    MSAARTDesc.Width = Width;
    MSAARTDesc.DepthOrArraySize = 1;
    MSAARTDesc.MipLevels = 1;
    MSAARTDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    MSAARTDesc.SampleDesc.Count = RendererConfig::MSAACount;
    MSAARTDesc.SampleDesc.Quality = 0;
    MSAARTDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    MSAARTDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    D3D12_CLEAR_VALUE ClearValue{};
    ClearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    ClearValue.Color[0] = 0.0f;
    ClearValue.Color[1] = 0.0f;
    ClearValue.Color[2] = 0.0f;
    ClearValue.Color[3] = 1.0f;

    D3D12_HEAP_PROPERTIES MSAAHeapProps{};
    MSAAHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    DX_VALIDATE(
        ContextRef.GetDevice()->CreateCommittedResource(
            &MSAAHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &MSAARTDesc,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            &ClearValue,
            IID_PPV_ARGS(&MSAARTV)
        ),
        MSAARTV
    );

    D3D12_RENDER_TARGET_VIEW_DESC MSAARTVDesc{};
    MSAARTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    MSAARTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;

    ContextRef.GetDevice()->CreateRenderTargetView(MSAARTV.Get(), &MSAARTVDesc, MSAARTVHandle);
}

void DXSwapchain::ReleaseRTVHeaps()
{
    RTVDescHeap.Reset();
    RTVHandles.clear();
}

void DXSwapchain::ReleaseMSAARTVHeap()
{
    MSAARTV.Reset();
    MSAARTVDescHeap.Reset();
}

void DXSwapchain::CreateDSV()
{
    D3D12_DESCRIPTOR_HEAP_DESC DSVHeapDesc{};
    DSVHeapDesc.NumDescriptors = 1;
    DSVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    DSVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DX_VALIDATE(ContextRef.GetDevice()->CreateDescriptorHeap(&DSVHeapDesc, IID_PPV_ARGS(&DSVHeap)), DSVHeap);

    DSVHandle = DSVHeap->GetCPUDescriptorHandleForHeapStart();
}

void DXSwapchain::CreateMSAADSVAndBuffer()
{
    D3D12_RESOURCE_DESC MSAADepthDesc{};
    MSAADepthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    MSAADepthDesc.Width = Width;
    MSAADepthDesc.Height = Height;
    MSAADepthDesc.DepthOrArraySize = 1;
    MSAADepthDesc.MipLevels = 1;
    MSAADepthDesc.Format = DXGI_FORMAT_D32_FLOAT;
    MSAADepthDesc.SampleDesc.Count = RendererConfig::MSAACount;
    MSAADepthDesc.SampleDesc.Quality = 0;
    MSAADepthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    MSAADepthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    D3D12_CLEAR_VALUE ClearValue{};
    ClearValue.Format = DXGI_FORMAT_D32_FLOAT;
    ClearValue.DepthStencil.Depth = 1.0f;
    ClearValue.DepthStencil.Stencil = 0;

    D3D12_HEAP_PROPERTIES MSAAHeapProps{};
    MSAAHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    DX_VALIDATE(
        ContextRef.GetDevice()->CreateCommittedResource(
            &MSAAHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &MSAADepthDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &ClearValue,
            IID_PPV_ARGS(&MSAADepthStencilBuffer)
        ),
        MSAADepthStencilBuffer
    );

    D3D12_DESCRIPTOR_HEAP_DESC DsvHeapDesc{};
    DsvHeapDesc.NumDescriptors = 1;
    DsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    DsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DX_VALIDATE(ContextRef.GetDevice()->CreateDescriptorHeap(&DsvHeapDesc, IID_PPV_ARGS(&MSAADSVHeap)), MSAADSVHeap);

    MSAADSVHandle = MSAADSVHeap->GetCPUDescriptorHandleForHeapStart();
    ContextRef.GetDevice()->CreateDepthStencilView(MSAADepthStencilBuffer.Get(), nullptr, MSAADSVHandle);
}

void DXSwapchain::CreateDepthStencilBuffer()
{
    D3D12_RESOURCE_DESC DepthDesc{};
    DepthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    DepthDesc.Width = Width;
    DepthDesc.Height = Height;
    DepthDesc.DepthOrArraySize = 1;
    DepthDesc.MipLevels = 1;
    DepthDesc.Format = DXGI_FORMAT_D32_FLOAT;
    DepthDesc.SampleDesc.Count = RendererConfig::MSAACount;
    DepthDesc.SampleDesc.Quality = 0;
    DepthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    DepthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    D3D12_CLEAR_VALUE ClearValue{};
    ClearValue.Format = DXGI_FORMAT_D32_FLOAT;
    ClearValue.DepthStencil.Depth = 1.0f;
    ClearValue.DepthStencil.Stencil = 0;

    D3D12_HEAP_PROPERTIES HeapProps{};
    HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    HeapProps.CreationNodeMask = 1;
    HeapProps.VisibleNodeMask = 1;

    DX_VALIDATE(
        ContextRef.GetDevice()->CreateCommittedResource(
            &HeapProps,
            D3D12_HEAP_FLAG_NONE,
            &DepthDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &ClearValue,
            IID_PPV_ARGS(&DepthStencilBuffer)
        ),
        DepthStencilBuffer
    );

    ContextRef.GetDevice()->CreateDepthStencilView(DepthStencilBuffer.Get(), nullptr, DSVHandle);
}

void DXSwapchain::ReleaseDSV()
{
    DepthStencilBuffer.Reset();
    DSVHeap.Reset();
}

void DXSwapchain::MSAAReleaseDSV()
{
    MSAADepthStencilBuffer.Reset();
    MSAADSVHeap.Reset();
}

void DXSwapchain::InitializeViews()
{
    ReleaseDSV();
    ReleaseRTVHeaps();
    ReleaseMSAARTVHeap();
    GetBuffers();
    CreateRTVAndDescHeap();
    CreateMSAARTVAndDescHeap();
    CreateDSV();
    CreateDepthStencilBuffer();
    CreateMSAADSVAndBuffer();
}

void DXSwapchain::ResolveToBackBuffers(ID3D12GraphicsCommandList7 *CmdList)
{
    // transtion msaa and current back buffer to resolve state
    Utils::TransitionResoure(
        CmdList, MSAARTV.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_RESOLVE_SOURCE
    );

    Utils::TransitionResoure(
        CmdList, GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RESOLVE_DEST
    );

    CmdList->ResolveSubresource(GetCurrentBackBuffer(), 0, MSAARTV.Get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);

    // transition backbuffer for ui rendering
    Utils::TransitionResoure(
        CmdList, GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET
    );

    Utils::TransitionResoure(
        CmdList, MSAARTV.Get(), D3D12_RESOURCE_STATE_RESOLVE_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET
    );
}