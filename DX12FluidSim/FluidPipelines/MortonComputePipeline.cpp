#include "FluidPipelines/MortonComputePipeline.hpp"
#include "FluidPipelines/FluidPipelinesHeapDesc.hpp"
#include "DebugLayer/DebugMacros.hpp"

MortonComputePipeline::MortonComputePipeline(ID3D12Device14 &Device) : FluidComputePipelineBase(Device) {}

MortonComputePipeline::~MortonComputePipeline() = default;

void MortonComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    UINT StructuredBufferSize = sizeof(MortonSB) * ParticleCount;
    std::vector<MortonSB> particleData(ParticleCount);

    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        StructuredBufferSize,
        particleData.data(),
        MortonGPU.DefaultBuffer,
        MortonGPU.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );
}

void MortonComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc)
{
    MortonUAVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::UAV, MortonGPU.DefaultBuffer, ParticleCount, sizeof(MortonSB));

    MortonSRVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::SRV, MortonGPU.DefaultBuffer, ParticleCount, sizeof(MortonSB));
}