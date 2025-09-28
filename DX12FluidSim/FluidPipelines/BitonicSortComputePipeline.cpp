#include "FluidPipelines/BitonicSortComputePipeline.hpp"
#include "Shared/SimData.hpp"

BitonicSortComputePipeline::BitonicSortComputePipeline(ID3D12Device14 &Device) : FluidComputePipelineBase(Device) {}

void BitonicSortComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    UINT StructuredBufferSize = sizeof(SortedHashData) * SimInitials::ParticleCount;
    std::vector<SortedHashData> Data(SimInitials::ParticleCount);

    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        StructuredBufferSize,
        Data.data(),
        BitonicSortGPUResources.DefaultBuffer,
        BitonicSortGPUResources.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );
}

void BitonicSortComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc)
{
    BitonicSortUAVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::UAV, BitonicSortGPUResources.DefaultBuffer, SimInitials::ParticleCount, sizeof(SortedHashData)
    );

    BitonicSortSRVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::SRV, BitonicSortGPUResources.DefaultBuffer, SimInitials::ParticleCount, sizeof(SortedHashData)
    );
}
