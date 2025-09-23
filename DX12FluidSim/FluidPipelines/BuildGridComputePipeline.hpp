#pragma once

#include "FluidPipelines/FluidComputePipelineBase.hpp"

struct CellStartGPU
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> UploadBuffer;
};

struct CellEndGPU
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> UploadBuffer;
};

class BuildGridComputePipeline : public FluidComputePipelineBase
{
public:
    BuildGridComputePipeline(ID3D12Device14 &Device);
    ~BuildGridComputePipeline() = default;

    BuildGridComputePipeline(const BuildGridComputePipeline &) = delete;
    BuildGridComputePipeline &operator=(const BuildGridComputePipeline &) = delete;

    
    virtual void CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList) override;
    virtual void CreateBufferDesc(FluidHeapDescriptor &HeapDesc) override;


    D3D12_GPU_DESCRIPTOR_HANDLE GetCellStartUAVGPUHandle() const { return CellStartUAVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetCellStartSRVGPUHandle() const { return CellStartSRVGPUHandle; }
   
    D3D12_GPU_DESCRIPTOR_HANDLE GetCellEndUAVGPUHandle() const { return CellEndUAVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetCellEndSRVGPUHandle() const { return CellEndSRVGPUHandle; }

    ID3D12Resource2 *GetCellStartBuffer() const { return CellStartGPUResources.DefaultBuffer.Get(); }
    ID3D12Resource2 *GetCellEndBuffer() const { return CellEndGPUResources.DefaultBuffer.Get(); }

private:
    CellStartGPU CellStartGPUResources;
    CellEndGPU CellEndGPUResources;

    D3D12_GPU_DESCRIPTOR_HANDLE CellStartUAVGPUHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE CellStartSRVGPUHandle;

    D3D12_GPU_DESCRIPTOR_HANDLE CellEndUAVGPUHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE CellEndSRVGPUHandle;
};