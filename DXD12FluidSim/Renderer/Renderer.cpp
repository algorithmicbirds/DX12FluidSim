#include "Renderer/Renderer.hpp"
#include "D3D/DXSwapchain.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "D3D/DXPipeline.hpp"

Renderer::Renderer(DXSwapchain &Swapchain, ID3D12Device &Device) : SwapchainRef(Swapchain), DeviceRef(Device)
{
    Init();
}
Renderer::~Renderer() { 
    ReleaseRTVHeaps();
    VertexBuffer_Default.Reset();
}

void Renderer::Init()
{
    CreateRTVAndDescHeap();
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

void Renderer::InitializeBuffers(ID3D12GraphicsCommandList7 *CmdList) {
  
    UINT VertexBufferSize = sizeof(TriangleVertices);
    VertexBuffer_Default =
        CreateVertexBuffer(VertexBufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST);
    VertexBuffer_Upload =
        CreateVertexBuffer(VertexBufferSize, D3D12_HEAP_TYPE_GPU_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
 
    Vertex *mappedData = nullptr;
    VertexBuffer_Upload->Map(0, nullptr, reinterpret_cast<void **>(&mappedData));
    memcpy(mappedData, TriangleVertices, sizeof(TriangleVertices));
    VertexBuffer_Upload->Unmap(0, nullptr);

    CmdList->CopyBufferRegion(VertexBuffer_Default.Get(), 0, VertexBuffer_Upload.Get(), 0, VertexBufferSize);
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

void Renderer::BindInputAssembler(ID3D12GraphicsCommandList7 *CmdList)
{
    D3D12_VERTEX_BUFFER_VIEW VBV{};
    VBV.BufferLocation = VertexBuffer_Default->GetGPUVirtualAddress();
    VBV.SizeInBytes = sizeof(TriangleVertices);
    VBV.StrideInBytes = sizeof(Vertex);
    CmdList->IASetVertexBuffers(0, 1, &VBV);
    CmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    CmdList->DrawInstanced(_countof(TriangleVertices), 1, 0, 0);
}
