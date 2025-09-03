#pragma once

#include "GlobInclude/WinInclude.hpp"

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
    D3D12_RESOURCE_STATES InitialResourceState
);
void CreateUploadBuffer(
    ID3D12Device14 &Device,
    ID3D12GraphicsCommandList7 *CmdList,
    UINT BufferSize,
    const void *CPUData,
    ComPtr<ID3D12Resource2> &DefaultBuffer,
    ComPtr<ID3D12Resource2> &UploadBuffer
);
} // namespace Utils