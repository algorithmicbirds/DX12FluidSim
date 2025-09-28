#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Shared/SimData.hpp"
#include "FluidPipelines/FluidComputePipelineBase.hpp"

#include <string>
#include <vector>

class DebugComputePipeline : public FluidComputePipelineBase
{
private:
    struct HashData
    {
        UINT HashCode;
        UINT ParticleIndex;
    };

    struct DebugGPUResources
    {
        ComPtr<ID3D12Resource2> DefaultBuffer;
        ComPtr<ID3D12Resource2> UploadBuffer;
    };

public:
    DebugComputePipeline(ID3D12Device14 &Device);
    ~DebugComputePipeline() = default;

    DebugComputePipeline&operator=(const DebugComputePipeline&) = delete;
    DebugComputePipeline(const DebugComputePipeline&) = delete;

public:
    virtual void CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList) override;
    virtual void CreateBufferDesc(FluidHeapDescriptor &HeapDesc) override;
    ID3D12Resource2 *GetHashBuffer() const { return DebugGPU.DefaultBuffer.Get(); }

private:
    DebugGPUResources DebugGPU;
    UINT ParticleCount = SimInitials::ParticleCount;

};