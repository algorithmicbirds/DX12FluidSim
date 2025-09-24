#include "FluidPipelines/RadixHistogramComputePipeline.hpp"
#include "DebugLayer/DebugMacros.hpp"

RadixHistogramComputePipeline::RadixHistogramComputePipeline(ID3D12Device14 &Device) : FluidComputePipelineBase(Device) {}

void RadixHistogramComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    NumBins = 1u << ParticleInitialValues::RadixNumBits;
    UINT StructuredBufferSize = sizeof(UINT) * NumBins;
    std::vector<UINT> particleData(NumBins, 0);

    RadixHistogramCB.Initialize(DeviceRef);
    RadixHistogramCBV RadixHistogramData{};
    RadixHistogramData.ParticleCount = ParticleCount;
    RadixHistogramData.NumBits = ParticleInitialValues::RadixNumBits;
    RadixHistogramData.Shift = 0;
    RadixHistogramCB.Update(RadixHistogramData);

    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        StructuredBufferSize,
        particleData.data(),
        RadixHistogramGPU.DefaultBuffer,
        RadixHistogramGPU.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );
}

void RadixHistogramComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc)
{
    RadixHistogramUAVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::UAV, RadixHistogramGPU.DefaultBuffer, NumBins, sizeof(UINT));

    RadixHistogramSRVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::SRV, RadixHistogramGPU.DefaultBuffer, NumBins, sizeof(UINT));
}