#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Shared/SimData.hpp"
#include "FluidPipelines/FluidComputePipelineBase.hpp"

#include <string>
#include <vector>

struct MortonSB
{
    DirectX::XMFLOAT3 Positions;
};

struct MortonGPUResources
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> UploadBuffer;
};

class MortonComputePipeline : public FluidComputePipelineBase
{
public:
    MortonComputePipeline(ID3D12Device14 &Device);
    ~MortonComputePipeline();

    MortonComputePipeline &operator=(const MortonComputePipeline &) = delete;
    MortonComputePipeline(const MortonComputePipeline &) = delete;

public:
    virtual void CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList) override;
    virtual void CreateBufferDesc(FluidHeapDescriptor &HeapDesc) override;

    D3D12_GPU_DESCRIPTOR_HANDLE GetMortonUAVGPUHandle() const { return MortonUAVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetMortonSRVGPUHandle() const { return MortonSRVGPUHandle; }
    ID3D12Resource2 *GetMortonBuffer() const { return MortonGPU.DefaultBuffer.Get(); }

private:
    MortonGPUResources MortonGPU;
    UINT ParticleCount = SimInitials::ParticleCount;

    D3D12_GPU_DESCRIPTOR_HANDLE MortonUAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE MortonSRVGPUHandle{};
};