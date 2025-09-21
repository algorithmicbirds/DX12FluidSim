#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Shared/Utils.hpp"
#include "FluidPipelines/FluidPipelinesHeapDesc.hpp"

#include <string>
#include <vector>

class FluidComputePipelineBase
{
public:
    explicit FluidComputePipelineBase(ID3D12Device14 &Device);
    virtual ~FluidComputePipelineBase();

    FluidComputePipelineBase(const FluidComputePipelineBase &) = delete;
    FluidComputePipelineBase &operator=(const FluidComputePipelineBase &) = delete;
    FluidComputePipelineBase(FluidComputePipelineBase &&) = delete;
    FluidComputePipelineBase &operator=(FluidComputePipelineBase &&) = delete;

    void SetRootSignature(ComPtr<ID3D12RootSignature> InRootSig);
    void CreatePipeline(const std::string &CSFilePath, FluidHeapDescriptor &HeapDesc);
    void BindRootAndPSO(ID3D12GraphicsCommandList7 *CmdList);

    virtual void CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList) = 0;
    virtual void CreateBufferDesc(FluidHeapDescriptor &HeapDesc) = 0;

protected:
    void CreatePipelineState(const std::vector<char> &CSCode);

    ID3D12Device14 &DeviceRef;
    ComPtr<ID3D12RootSignature> RootSignature;
    ComPtr<ID3D12PipelineState> PipelineStateObject;
};