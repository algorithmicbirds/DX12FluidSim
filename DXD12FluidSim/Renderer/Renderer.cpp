#include "Renderer/Renderer.hpp"
#include "D3D/DXSwapchain.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "D3D/DXPipeline.hpp"
#include <iostream>

Renderer::Renderer(DXSwapchain &Swapchain, ID3D12Device14 &Device) : SwapchainRef(Swapchain), DeviceRef(Device)
{
    Pipeline = std::make_unique<DXPipeline>(DeviceRef, SHADER_PATH "Triangle_vs.cso", SHADER_PATH "Triangle_ps.cso");
    CreateRTVAndDescHeap();
}
Renderer::~Renderer()
{
    ReleaseRTVHeaps();
    VertexBuffer_Default.Reset();
}

void Renderer::BeginFrame(ID3D12GraphicsCommandList7 *CmdList)
{
    ID3D12Resource1 *CurrentBuffer = SwapchainRef.GetCurrentBackBuffer();
    VALIDATE_PTR(CurrentBuffer);

    TransitionResoure(CmdList, CurrentBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

    float ClearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    UINT CurrentBackBufferIndex = SwapchainRef.GetCurrentBackBufferIndex();

    CmdList->ClearRenderTargetView(RTVHandles.at(CurrentBackBufferIndex), ClearColor, 0, nullptr);
    CmdList->OMSetRenderTargets(1, &RTVHandles.at(CurrentBackBufferIndex), false, nullptr);
    CmdList->SetPipelineState(Pipeline->GetPipelineStateObject());
    CmdList->SetGraphicsRootSignature(Pipeline->GetRootSignature());
    CmdList->IASetVertexBuffers(0, 1, &VertexBufferView);
    CmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    CmdList->RSSetViewports(1, &SwapchainRef.GetViewport());
    CmdList->RSSetScissorRects(1, &SwapchainRef.GetScissorRect());
    CmdList->DrawInstanced(_countof(TriangleVertices), 1, 0, 0);
}

void Renderer::EndFrame(ID3D12GraphicsCommandList7 *CmdList)
{
    ID3D12Resource1 *CurrentBuffer = SwapchainRef.GetCurrentBackBuffer();
    VALIDATE_PTR(CurrentBuffer);
    TransitionResoure(CmdList, CurrentBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
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

void Renderer::InitializeBuffers(ID3D12GraphicsCommandList7 *CmdList)
{

    UINT VertexBufferSize = sizeof(TriangleVertices);
    VertexBuffer_Default =
        CreateVertexBuffer(VertexBufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
    VertexBuffer_Upload =
        CreateVertexBuffer(VertexBufferSize, D3D12_HEAP_TYPE_GPU_UPLOAD, D3D12_RESOURCE_STATE_COMMON);

    TransitionResoure(CmdList, VertexBuffer_Default.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
    Vertex *mappedData = nullptr;
    VertexBuffer_Upload->Map(0, nullptr, reinterpret_cast<void **>(&mappedData));
    memcpy(mappedData, TriangleVertices, sizeof(TriangleVertices));
    VertexBuffer_Upload->Unmap(0, nullptr);
    CmdList->CopyBufferRegion(VertexBuffer_Default.Get(), 0, VertexBuffer_Upload.Get(), 0, VertexBufferSize);
    TransitionResoure(
        CmdList,
        VertexBuffer_Default.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
    );

    CreateVertexBufferView();
}

void Renderer::ReleaseRTVHeaps()
{
    RTVDescHeap.Reset();
    RTVHandles.clear();
}

ComPtr<ID3D12Resource2> Renderer::CreateVertexBuffer(
    UINT64 SizeOfBufferInBytes, D3D12_HEAP_TYPE HeapType, D3D12_RESOURCE_STATES InitialResourceState
)
{
    D3D12_HEAP_PROPERTIES HeapProps{};
    HeapProps.Type = HeapType;
    HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    HeapProps.CreationNodeMask = 1;
    HeapProps.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC Desc{};
    Desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    Desc.Format = DXGI_FORMAT_UNKNOWN;
    Desc.Alignment = 0;
    Desc.MipLevels = 1;
    Desc.Width = SizeOfBufferInBytes;
    Desc.Height = 1;
    Desc.DepthOrArraySize = 1;
    Desc.SampleDesc.Count = 1;
    Desc.SampleDesc.Quality = 0;
    Desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    Desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    ComPtr<ID3D12Resource2> Buffer;

    DX_VALIDATE(
        DeviceRef.CreateCommittedResource(
            &HeapProps, D3D12_HEAP_FLAG_NONE, &Desc, InitialResourceState, nullptr, IID_PPV_ARGS(&Buffer)
        ),
        Buffer
    );
    return Buffer;
}

void Renderer::CreateVertexBufferView()
{
    VertexBufferView.BufferLocation = VertexBuffer_Default->GetGPUVirtualAddress();
    VertexBufferView.SizeInBytes = sizeof(TriangleVertices);
    VertexBufferView.StrideInBytes = sizeof(Vertex);
}

void Renderer::TransitionResoure(
    ID3D12GraphicsCommandList7 *CmdList,
    ID3D12Resource *ResourceToTransition,
    D3D12_RESOURCE_STATES BeforeState,
    D3D12_RESOURCE_STATES AfterState
)
{
    D3D12_RESOURCE_BARRIER Barrier{};
    Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    Barrier.Transition.pResource = ResourceToTransition;
    Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    Barrier.Transition.StateBefore = BeforeState;
    Barrier.Transition.StateAfter = AfterState;
    CmdList->ResourceBarrier(1, &Barrier);
}
