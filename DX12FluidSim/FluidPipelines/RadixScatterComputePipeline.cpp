#include "RadixScatterComputePipeline.hpp"

RadixScatterComputePipeline::RadixScatterComputePipeline(ID3D12Device14 &Device) : FluidComputePipelineBase(Device) {}

void RadixScatterComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    NumBins = 1u << ParticleInitialValues::RadixNumBits;
    UINT BinOffsetSBSize = sizeof(UINT) * NumBins;
    std::vector<UINT> BinOffsetData(NumBins, 0);
    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        BinOffsetSBSize,
        BinOffsetData.data(),
        RadixScatterBinOffsetGPU.DefaultBuffer,
        RadixScatterBinOffsetGPU.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );

    UINT ScatterSBSize = sizeof(KeyValue) * ParticleCount;
    std::vector<KeyValue> ScatterData(ParticleCount);
    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        ScatterSBSize,
        ScatterData.data(),
        RadixScatterGPU.DefaultBuffer,
        RadixScatterGPU.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );
}

void RadixScatterComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc)
{
    RadixScatterUAVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::UAV, RadixScatterGPU.DefaultBuffer, ParticleCount, sizeof(KeyValue)
    );

    RadixScatterSRVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::SRV, RadixScatterGPU.DefaultBuffer, ParticleCount, sizeof(KeyValue)
    );

    RadixScatterBinOffsetsUAVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::UAV, RadixScatterBinOffsetGPU.DefaultBuffer, NumBins, sizeof(UINT));
}
