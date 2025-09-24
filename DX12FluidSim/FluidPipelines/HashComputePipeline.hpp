#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Shared/SimData.hpp"
#include "FluidPipelines/FluidComputePipelineBase.hpp"

#include <string>
#include <vector>

class HashComputePipeline : public FluidComputePipelineBase
{
private:
    struct HashGPUResources
    {
        ComPtr<ID3D12Resource2> DefaultBuffer;
        ComPtr<ID3D12Resource2> ReadBackBuffer;
    };
    struct HashDataSB
    {
        UINT HashCode;
        UINT ParticleIndex;
    };

public:
    HashComputePipeline(ID3D12Device14 &Device);
    ~HashComputePipeline();

    HashComputePipeline &operator=(const HashComputePipeline &) = delete;
    HashComputePipeline(const HashComputePipeline &) = delete;

public:
    virtual void CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList) override;
    virtual void CreateBufferDesc(FluidHeapDescriptor &HeapDesc) override;
    void ReadBackHashBuffer(ID3D12GraphicsCommandList7 *CmdList);
    void SortHashedValues();
    D3D12_GPU_DESCRIPTOR_HANDLE GetHashUAVGPUHandle() const { return HashUAVGPUHandle; }
    /* D3D12_GPU_DESCRIPTOR_HANDLE GetHashSRVGPUHandle() const { return HashSRVGPUHandle; }*/
    ID3D12Resource2 *GetHashBuffer() const { return HashGPU.DefaultBuffer.Get(); }

private:
    HashGPUResources HashGPU;
    UINT ParticleCount = SimInitials::ParticleCount;
    std::vector<HashDataSB> HashSBCPU;
    D3D12_GPU_DESCRIPTOR_HANDLE HashUAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE HashSRVGPUHandle{};
};