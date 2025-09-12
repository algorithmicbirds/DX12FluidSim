#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>
#include <string>


enum class DescriptorType
{
    UAV,
    SRV
};

namespace Utils
{
void TransitionResoure(
    ID3D12GraphicsCommandList7 *CmdList,
    ID3D12Resource *ResourceToTransition,
    D3D12_RESOURCE_STATES BeforeState,
    D3D12_RESOURCE_STATES AfterState
);
ComPtr<ID3D12Resource2> CreateBuffer(
    ID3D12Device14 &Device,
    UINT64 SizeOfBufferInBytes,
    D3D12_HEAP_TYPE HeapType,
    D3D12_RESOURCE_STATES InitialResourceState = D3D12_RESOURCE_STATE_COMMON,
    D3D12_RESOURCE_FLAGS Flag = D3D12_RESOURCE_FLAG_NONE
);
void CreateUploadBuffer(
    ID3D12Device14 &Device,
    ID3D12GraphicsCommandList7 *CmdList,
    UINT BufferSize,
    const void *CPUData,
    ComPtr<ID3D12Resource2> &DefaultBuffer,
    ComPtr<ID3D12Resource2> &UploadBuffer,
    D3D12_RESOURCE_FLAGS Flag = D3D12_RESOURCE_FLAG_NONE,
    D3D12_RESOURCE_STATES InitialResourceState = D3D12_RESOURCE_STATE_COMMON
);
void CreateDynamicUploadBuffer(
    ID3D12Device14 &Device, UINT BufferSize, ComPtr<ID3D12Resource2> &UploadBuffer, void *&MappedPtr
);
D3D12_GPU_DESCRIPTOR_HANDLE CreateBufferDescriptor(
    ID3D12Device14 &Device,
    DescriptorType DescType,
    ComPtr<ID3D12Resource2> DefaultBuffer,
    ComPtr<ID3D12DescriptorHeap> DescHeap,
    UINT NumOfElems,
    UINT Stride,
    UINT DescriptorIndex = 0
);
D3D12_GPU_DESCRIPTOR_HANDLE CreateTextureDescriptor(
    ID3D12Device14 &Device,
    DescriptorType DescType,
    ComPtr<ID3D12Resource2> Texture,
    ComPtr<ID3D12DescriptorHeap> DescHeap,
    DXGI_FORMAT Format,
    UINT DescriptorIndex = 0
);
std::vector<char> ReadFile(const std::string &FilePath);
} // namespace Utils