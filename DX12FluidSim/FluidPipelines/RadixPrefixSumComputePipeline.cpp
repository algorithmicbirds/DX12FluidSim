#include "RadixPrefixSumComputePipeline.hpp"

RadixPrefixSumComputePipeline::RadixPrefixSumComputePipeline(ID3D12Device14 &Device) : FluidComputePipelineBase(Device)
{
}

void RadixPrefixSumComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    NumBins = 1u << ParticleInitialValues::RadixNumBits;
    UINT StructuredBufferSize = sizeof(UINT) * NumBins;
    std::vector<UINT> particleData(ParticleCount);

    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        StructuredBufferSize,
        particleData.data(),
        RadixPrefixSumGPU.DefaultBuffer,
        RadixPrefixSumGPU.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );
}

void RadixPrefixSumComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc) {
    RadixPrefixSumUAVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::UAV, RadixPrefixSumGPU.DefaultBuffer, NumBins, sizeof(UINT));

    RadixPrefixSumSRVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::SRV, RadixPrefixSumGPU.DefaultBuffer, NumBins, sizeof(UINT));
}
