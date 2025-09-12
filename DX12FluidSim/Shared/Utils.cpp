#include "Shared/Utils.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include <fstream>

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
    D3D12_RESOURCE_STATES InitialResourceState,
    D3D12_RESOURCE_FLAGS Flags
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
    Desc.Flags = Flags;

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
    ComPtr<ID3D12Resource2> &UploadBuffer,
    D3D12_RESOURCE_FLAGS Flag,
    D3D12_RESOURCE_STATES InitialResourceState
)
{
    DefaultBuffer = CreateBuffer(Device, BufferSize, D3D12_HEAP_TYPE_DEFAULT, InitialResourceState, Flag);
    UploadBuffer = CreateBuffer(Device, BufferSize, D3D12_HEAP_TYPE_GPU_UPLOAD, InitialResourceState, Flag);

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
D3D12_GPU_DESCRIPTOR_HANDLE CreateBufferDescriptor(
    ID3D12Device14 &Device,
    DescriptorType DescType,
    ComPtr<ID3D12Resource2> DefaultBuffer,
    ComPtr<ID3D12DescriptorHeap> DescHeap,
    UINT NumOfElems,
    UINT Stride,
    UINT DescriptorIndex
)
{
    UINT HandleSize = Device.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = DescHeap->GetGPUDescriptorHandleForHeapStart();
    GPUHandle.ptr += DescriptorIndex * HandleSize;

    D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = DescHeap->GetCPUDescriptorHandleForHeapStart();
    CPUHandle.ptr += DescriptorIndex * HandleSize;

    if (DescType == DescriptorType::UAV)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC Desc{};
        Desc.Format = DXGI_FORMAT_UNKNOWN;
        Desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        Desc.Buffer.NumElements = NumOfElems;
        Desc.Buffer.StructureByteStride = Stride;
        Desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

        Device.CreateUnorderedAccessView(
            DefaultBuffer.Get(), nullptr, &Desc, CPUHandle
        );
    }
    else if (DescType == DescriptorType::SRV)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC Desc{};
        Desc.Format = DXGI_FORMAT_UNKNOWN;
        Desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        Desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        Desc.Buffer.NumElements = NumOfElems;
        Desc.Buffer.StructureByteStride = Stride;
        Desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        Device.CreateShaderResourceView(DefaultBuffer.Get(), &Desc, CPUHandle);
    }

    return GPUHandle;
}
D3D12_GPU_DESCRIPTOR_HANDLE CreateTextureDescriptor(
    ID3D12Device14 &Device,
    DescriptorType DescType,
    ComPtr<ID3D12Resource2> Texture,
    ComPtr<ID3D12DescriptorHeap> DescHeap,
    DXGI_FORMAT Format,
    UINT DescriptorIndex
)
{
    UINT HandleSize = Device.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = DescHeap->GetGPUDescriptorHandleForHeapStart();
    GPUHandle.ptr += DescriptorIndex * HandleSize;

    D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = DescHeap->GetCPUDescriptorHandleForHeapStart();
    CPUHandle.ptr += DescriptorIndex * HandleSize;

    if (DescType == DescriptorType::UAV)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC Desc{};
        Desc.Format = Format;
        Desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        Desc.Texture2D.MipSlice = 0;
        Desc.Texture2D.PlaneSlice = 0;

        Device.CreateUnorderedAccessView(Texture.Get(), nullptr, &Desc, CPUHandle);
    }
    else if (DescType == DescriptorType::SRV)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC Desc{};
        Desc.Format = Format;
        Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        Desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        Desc.Texture2D.MostDetailedMip = 0;
        Desc.Texture2D.MipLevels = 1;
        Desc.Texture2D.PlaneSlice = 0;
        Desc.Texture2D.ResourceMinLODClamp = 0.0f;

        Device.CreateShaderResourceView(Texture.Get(), &Desc, CPUHandle);
    }

    return GPUHandle;
}
std::vector<char> ReadFile(const std::string &FilePath)
{
    std::ifstream File{FilePath, std::ios::ate | std::ios::binary};
    if (!File.is_open())
    {
        throw std::runtime_error("File failed to open");
    }

    size_t FileSize = static_cast<size_t>(File.tellg());
    std::vector<char> Buffer(FileSize);
    File.seekg(0);
    File.read(Buffer.data(), FileSize);
    File.close();

    return Buffer;
}
} // namespace Utils
