#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Shared/SimData.hpp"
#include "FluidPipelines/FluidPipelinesHeapDesc.hpp"

struct PixelGPUDebugResources
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> ReadBackBuffer;
};

struct PixelDebugStructuredBuffer
{
    float Density;
};

class StupidDebugBuffer
{
public:
    void ReadBackDebugBuffer(ID3D12GraphicsCommandList7 *CmdList);
    void CreateDebugUAVDesc(ID3D12Device14 &Device, FluidHeapDescriptor &HeapDesc);
    D3D12_GPU_DESCRIPTOR_HANDLE GetDebugGPUDescHandle() const { return DebugGPUDescHandle; }

private:
    PixelGPUDebugResources GPUDebug;
    UINT ParticleCount = SimInitials::ParticleCount;

    D3D12_GPU_DESCRIPTOR_HANDLE DebugGPUDescHandle;
};
