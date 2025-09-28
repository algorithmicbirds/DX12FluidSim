#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Shared/SimData.hpp"
#include "FluidPipelines/FluidPipelinesHeapDesc.hpp"

class StupidDebugBuffer
{
private:
    struct GPUDebugResources
    {
        ComPtr<ID3D12Resource2> DefaultBuffer;
        ComPtr<ID3D12Resource2> ReadBackBuffer;
    };

    struct DebugStructuredBuffer
    {
        UINT ParticleHash;
        UINT ParticleIndex;
    };

public:
    void ReadBackDebugBuffer(ID3D12GraphicsCommandList7 *CmdList);
    void CreateDebugUAVDesc(ID3D12Device14 &Device, FluidHeapDescriptor &HeapDesc);
    D3D12_GPU_DESCRIPTOR_HANDLE GetDebugGPUDescHandle() const { return DebugGPUDescHandle; }

private:
    GPUDebugResources GPUDebug;
    UINT ParticleCount = SimInitials::ParticleCount;

    D3D12_GPU_DESCRIPTOR_HANDLE DebugGPUDescHandle;
};
