#include "Renderer/Renderer.hpp"
#include "D3D/DXSwapchain.hpp"
#include "DebugLayer/DebugMacros.hpp"

Renderer::Renderer(DXSwapchain &Swapchain, ID3D12Device &Device) : SwapchainRef(Swapchain), DeviceRef(Device)
{
    Init();
}
Renderer::~Renderer() { ReleaseRTVHeaps(); }

void Renderer::Init() { CreateRTVAndDescHeap(); }

void Renderer::BeginFrame(ID3D12GraphicsCommandList7 *CmdList)
{
    ID3D12Resource1 *CurrentBuffer = SwapchainRef.GetCurrentBackBuffer();
    VALIDATE_PTR(CurrentBuffer);

    D3D12_RESOURCE_BARRIER Barrier{};
    Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    Barrier.Transition.pResource = CurrentBuffer;
    Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    CmdList->ResourceBarrier(1, &Barrier);

    float ClearColor[] = {0.8f, 0.3f, 1.0f, 1.0f};
    UINT CurrentBackBufferIndex = SwapchainRef.GetCurrentBackBufferIndex();

    CmdList->ClearRenderTargetView(RTVHandles.at(CurrentBackBufferIndex), ClearColor, 0, nullptr);
    CmdList->OMSetRenderTargets(1, &RTVHandles.at(CurrentBackBufferIndex), false, nullptr);
}

void Renderer::EndFrame(ID3D12GraphicsCommandList7 *CmdList)
{
    VALIDATE_PTR(CmdList);
    ID3D12Resource1 *CurrentBuffer = SwapchainRef.GetCurrentBackBuffer();
    VALIDATE_PTR(CurrentBuffer);

    D3D12_RESOURCE_BARRIER Barrier{};
    Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    Barrier.Transition.pResource = CurrentBuffer;
    Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    CmdList->ResourceBarrier(1, &Barrier);
}

void Renderer::CreateRTVAndDescHeap()
{
    ReleaseRTVHeaps();
    D3D12_DESCRIPTOR_HEAP_DESC DescHeapDesc;
    DescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    DescHeapDesc.NumDescriptors = SwapchainRef.GetFrameCount();
    DescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DescHeapDesc.NodeMask = 0;
    DX_VALIDATE(DeviceRef.CreateDescriptorHeap(&DescHeapDesc, IID_PPV_ARGS(&RTVDescHeap)), RTVDescHeap);

    RTVHandles.resize(SwapchainRef.GetFrameCount());
    D3D12_CPU_DESCRIPTOR_HANDLE FirstCPUDescHandle = RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
    UINT HandleIncrement = DeviceRef.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    for (size_t i = 0; i < SwapchainRef.GetFrameCount(); ++i)
    {
        RTVHandles.at(i) = FirstCPUDescHandle;
        RTVHandles.at(i).ptr += HandleIncrement * i;
    }
    for (size_t i = 0; i < SwapchainRef.GetFrameCount(); ++i)
    {
        VALIDATE_PTR(RTVDescHeap);
        ID3D12Resource1 *buffer = SwapchainRef.GetBuffer(i);
        D3D12_RENDER_TARGET_VIEW_DESC RTV;
        RTV.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        RTV.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        RTV.Texture2D.MipSlice = 0;
        RTV.Texture2D.PlaneSlice = 0;

        DeviceRef.CreateRenderTargetView(buffer, &RTV, RTVHandles.at(i));
    }
}

void Renderer::ReleaseRTVHeaps()
{
    RTVDescHeap.Reset();
    RTVHandles.clear();
}