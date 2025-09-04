#include "GlobInclude/Utils.hpp"
#include "DebugLayer/DebugMacros.hpp"

namespace Utils
{
void TransitionResoure(
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
ComPtr<ID3D12Resource2> CreateBuffer(
    ID3D12Device14 &Device,
    UINT64 SizeOfBufferInBytes,
    D3D12_HEAP_TYPE HeapType,
    D3D12_RESOURCE_STATES InitialResourceState
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
        Device.CreateCommittedResource(
            &HeapProps, D3D12_HEAP_FLAG_NONE, &Desc, InitialResourceState, nullptr, IID_PPV_ARGS(&Buffer)
        ),
        Buffer
    );
    return Buffer;
}
void CreateUploadBuffer(
    ID3D12Device14 &Device,
    ID3D12GraphicsCommandList7 *CmdList,
    UINT BufferSize,
    const void *CPUData,
    ComPtr<ID3D12Resource2> &DefaultBuffer,
    ComPtr<ID3D12Resource2> &UploadBuffer
)
{
    DefaultBuffer = CreateBuffer(Device, BufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
    UploadBuffer = CreateBuffer(Device, BufferSize, D3D12_HEAP_TYPE_GPU_UPLOAD, D3D12_RESOURCE_STATE_COMMON);

    TransitionResoure(CmdList, DefaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
    void *mappedData = nullptr;
    UploadBuffer->Map(0, nullptr, reinterpret_cast<void **>(&mappedData));
    memcpy(mappedData, CPUData, BufferSize);
    UploadBuffer->Unmap(0, nullptr);
    CmdList->CopyBufferRegion(DefaultBuffer.Get(), 0, UploadBuffer.Get(), 0, BufferSize);
    TransitionResoure(
        CmdList, DefaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
    );
}

void CreateDynamicUploadBuffer(
    ID3D12Device14 &Device, UINT BufferSize, ComPtr<ID3D12Resource2> &UploadBuffer, void *&MappedPtr
)
{
    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC desc{};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = BufferSize;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    Device.CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&UploadBuffer)
    );

    UploadBuffer->Map(0, nullptr, reinterpret_cast<void **>(&MappedPtr));
}

}