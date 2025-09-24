#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Shared/SimData.hpp"
#include "FluidPipelines/FluidComputePipelineBase.hpp"
#include "Renderer/ConstantBuffers.hpp"

#include <string>
#include <vector>

class RadixHistogramComputePipeline : public FluidComputePipelineBase
{
private:
    struct RadixHistogramGPUResources
    {
        ComPtr<ID3D12Resource2> DefaultBuffer;
        ComPtr<ID3D12Resource2> UploadBuffer;
    };
    struct RadixHistogramCBV
    {
        UINT ParticleCount;
        UINT Shift;
        UINT NumBits;
    };

public:
    RadixHistogramComputePipeline(ID3D12Device14 &Device);
    ~RadixHistogramComputePipeline() = default;

    RadixHistogramComputePipeline &operator=(const RadixHistogramComputePipeline &) = delete;
    RadixHistogramComputePipeline(const RadixHistogramComputePipeline &) = delete;

public:
    virtual void CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList) override;
    virtual void CreateBufferDesc(FluidHeapDescriptor &HeapDesc) override;

    D3D12_GPU_DESCRIPTOR_HANDLE GetRadixHistogramUAVGPUHandle() const { return RadixHistogramUAVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetRadixHistogramSRVGPUHandle() const { return RadixHistogramSRVGPUHandle; }
    ID3D12Resource2 *GetRadixHistogramBuffer() const { return RadixHistogramGPU.DefaultBuffer.Get(); }
    D3D12_GPU_VIRTUAL_ADDRESS GetRadixHistogramCB() const { return RadixHistogramCB.GPUAddress; }

public:
    UINT NumBins = 0;

private:
    RadixHistogramGPUResources RadixHistogramGPU;
    UINT ParticleCount = SimInitials::ParticleCount;
    GPUConstantBuffer<RadixHistogramCBV> RadixHistogramCB;


    D3D12_GPU_DESCRIPTOR_HANDLE RadixHistogramUAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE RadixHistogramSRVGPUHandle{};
};