#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Shared/SimData.hpp"
#include "FluidPipelines/FluidComputePipelineBase.hpp"

#include <string>
#include <vector>

class HashComputePipeline : public FluidComputePipelineBase
{
private:
    struct GPUResources
    {
        ComPtr<ID3D12Resource2> DefaultBuffer;
        ComPtr<ID3D12Resource2> ReadBackBuffer;
        ComPtr<ID3D12Resource2> UploadBuffer;
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
    D3D12_GPU_DESCRIPTOR_HANDLE GetHashUAVGPUHandle() const { return HashUAVGPUHandle; }
    ID3D12Resource2 *GetHashBuffer() const { return HashGPUResources.DefaultBuffer.Get(); }
    void ReadSortUpdateHashBuffer(ID3D12GraphicsCommandList7 *CmdList);

private:
    void ReadBackHashBuffer(ID3D12GraphicsCommandList7 *CmdList);
    void SortHashData();
    void WriteSortedHashesToBuffer();
    void UploadSortedHashesToGPU(ID3D12GraphicsCommandList7 *CmdList);

private:
    GPUResources HashGPUResources;
    UINT ParticleCount = SimInitials::ParticleCount;
    std::vector<HashDataSB> HashSBCPU;
    D3D12_GPU_DESCRIPTOR_HANDLE HashUAVGPUHandle{};
};