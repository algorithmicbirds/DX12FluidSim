#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Shared/SimData.hpp"
#include "FluidPipelines/FluidComputePipelineBase.hpp"

#include <string>
#include <vector>

struct GridSB
{
    DirectX::XMFLOAT3 Positions;
};

struct GridGPUResources
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> UploadBuffer;
};

class GridComputePipeline : public FluidComputePipelineBase
{
public:
    GridComputePipeline(ID3D12Device14 &Device);
    ~GridComputePipeline();

    GridComputePipeline &operator=(const GridComputePipeline &) = delete;
    GridComputePipeline(const GridComputePipeline &) = delete;

public:
    virtual void CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList) override;
    ID3D12Resource2 *GetParticleIntegrateBuffer() const { return GridGPU.DefaultBuffer.Get(); }
    virtual void CreateBufferDesc(FluidHeapDescriptor &HeapDesc) override;

    D3D12_GPU_DESCRIPTOR_HANDLE GetGridUAVGPUHandle() const { return GridUAVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetGridSRVGPUHandle() const { return GridSRVGPUHandle; }

private:
    GridGPUResources GridGPU;
    UINT ParticleCount = SimInitials::ParticleCount;

    D3D12_GPU_DESCRIPTOR_HANDLE GridUAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE GridSRVGPUHandle{};
};