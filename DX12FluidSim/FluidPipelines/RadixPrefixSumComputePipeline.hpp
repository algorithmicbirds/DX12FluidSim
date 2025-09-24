#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Shared/SimData.hpp"
#include "FluidPipelines/FluidComputePipelineBase.hpp"

#include <string>
#include <vector>

class RadixPrefixSumComputePipeline : public FluidComputePipelineBase
{
private:
    struct RadixPrefixSumGPUResources
    {
        ComPtr<ID3D12Resource2> DefaultBuffer;
        ComPtr<ID3D12Resource2> UploadBuffer;
    };

public:
    RadixPrefixSumComputePipeline(ID3D12Device14 &Device);
    ~RadixPrefixSumComputePipeline() = default;

    RadixPrefixSumComputePipeline &operator=(const RadixPrefixSumComputePipeline &) = delete;
    RadixPrefixSumComputePipeline(const RadixPrefixSumComputePipeline &) = delete;

public:
    virtual void CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList) override;
    virtual void CreateBufferDesc(FluidHeapDescriptor &HeapDesc) override;

    D3D12_GPU_DESCRIPTOR_HANDLE GetRadixPrefixSumUAVGPUHandle() const { return RadixPrefixSumUAVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetRadixPrefixSumSRVGPUHandle() const { return RadixPrefixSumSRVGPUHandle; }
    ID3D12Resource2 *GetRadixPrefixSumBuffer() const { return RadixPrefixSumGPU.DefaultBuffer.Get(); }

private:
    RadixPrefixSumGPUResources RadixPrefixSumGPU;
    UINT ParticleCount = SimInitials::ParticleCount;

    UINT NumBins = 0;

    D3D12_GPU_DESCRIPTOR_HANDLE RadixPrefixSumUAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE RadixPrefixSumSRVGPUHandle{};
};