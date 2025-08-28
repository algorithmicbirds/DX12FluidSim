#include "Renderer/Renderer.hpp"
#include "D3D/DXSwapchain.hpp"
#include <stdexcept>
#include <iostream>
#include "DebugLayer/DebugMacros.hpp"

Renderer::Renderer(DXSwapchain &Swapchain, ID3D12Device &Device) : SwapchainRef(Swapchain), DeviceRef(Device)
{
    Init();
}
Renderer::~Renderer()
{
    RTVDescHeap.Reset();
}

void Renderer::Init()
{
    D3D12_DESCRIPTOR_HEAP_DESC DescHeapDesc;
    DescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    DescHeapDesc.NumDescriptors = SwapchainRef.GetFrameCount();
    DescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DescHeapDesc.NodeMask = 0;
    DX_VALIDATE(DeviceRef.CreateDescriptorHeap(&DescHeapDesc, IID_PPV_ARGS(&RTVDescHeap)), RTVDescHeap);
}

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
