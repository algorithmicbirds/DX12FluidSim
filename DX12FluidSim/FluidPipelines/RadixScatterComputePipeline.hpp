#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Shared/SimData.hpp"
#include "FluidPipelines/FluidComputePipelineBase.hpp"

#include <string>
#include <vector>

class RadixScatterComputePipeline : public FluidComputePipelineBase
{
private:
    struct RadixScatterGPUResources
    {
        ComPtr<ID3D12Resource2> DefaultBuffer;
        ComPtr<ID3D12Resource2> UploadBuffer;
    };
    struct KeyValue
    {
        UINT Key;
        UINT Value;
    };

public:
    RadixScatterComputePipeline(ID3D12Device14 &Device);
    ~RadixScatterComputePipeline() = default;

    RadixScatterComputePipeline &operator=(const RadixScatterComputePipeline &) = delete;
    RadixScatterComputePipeline(const RadixScatterComputePipeline &) = delete;

public:
    virtual void CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList) override;
    virtual void CreateBufferDesc(FluidHeapDescriptor &HeapDesc) override;

    D3D12_GPU_DESCRIPTOR_HANDLE GetRadixScatterUAVGPUHandle() const { return RadixScatterUAVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetRadixScatterSRVGPUHandle() const { return RadixScatterSRVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetRadixScatterBinOffsetsSRVGPUHandle() const
    {
        return RadixScatterBinOffsetsUAVGPUHandle;
    }
    ID3D12Resource2 *GetRadixScatterBuffer() const { return RadixScatterGPU.DefaultBuffer.Get(); }

private:
    RadixScatterGPUResources RadixScatterGPU;
    RadixScatterGPUResources RadixScatterBinOffsetGPU;
    UINT ParticleCount = SimInitials::ParticleCount;

    UINT NumBins = 0;

    D3D12_GPU_DESCRIPTOR_HANDLE RadixScatterUAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE RadixScatterBinOffsetsUAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE RadixScatterSRVGPUHandle{};
};