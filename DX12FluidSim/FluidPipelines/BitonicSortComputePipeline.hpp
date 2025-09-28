#pragma once

#include "FluidPipelines/FluidComputePipelineBase.hpp"

struct SortedHashData
{
    UINT HashCode;
    UINT ParticleIndex;
};

struct BitonicSortGPU
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> UploadBuffer;
};

class BitonicSortComputePipeline : public FluidComputePipelineBase
{
public:
    BitonicSortComputePipeline(ID3D12Device14 &Device);
    ~BitonicSortComputePipeline() = default;

    BitonicSortComputePipeline(const BitonicSortComputePipeline &) = delete;
    BitonicSortComputePipeline &operator=(const BitonicSortComputePipeline &) = delete;

    virtual void CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList) override;
    virtual void CreateBufferDesc(FluidHeapDescriptor &HeapDesc) override;

    D3D12_GPU_DESCRIPTOR_HANDLE GetBitonicSortUAVGPUHandle() const { return BitonicSortUAVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetBitonicSortSRVGPUHandle() const { return BitonicSortSRVGPUHandle; }

    ID3D12Resource2 *GetBitonicBuffer() const { return BitonicSortGPUResources.DefaultBuffer.Get(); }

private:
    D3D12_GPU_DESCRIPTOR_HANDLE BitonicSortUAVGPUHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE BitonicSortSRVGPUHandle;

    BitonicSortGPU BitonicSortGPUResources;
};