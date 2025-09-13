#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Shared/SimData.hpp"

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
    void CreateDebugUAVDesc(ID3D12Device14 &Device);
    D3D12_GPU_DESCRIPTOR_HANDLE GetDebugGPUDescHandle() const { return DebugGPUDescHandle; }
    void SetDescriptorHeap(ComPtr<ID3D12DescriptorHeap> DescHeap) { DebugDescHeap = DescHeap; }

private:
    PixelGPUDebugResources GPUDebug;
    UINT ParticleCount = SimInitials::ParticleCount;

    ComPtr<ID3D12DescriptorHeap> DebugDescHeap;

    D3D12_GPU_DESCRIPTOR_HANDLE DebugGPUDescHandle;
};
