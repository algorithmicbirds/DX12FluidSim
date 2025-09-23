#include "FluidPipelines/MortonComputePipeline.hpp"
#include "FluidPipelines/FluidPipelinesHeapDesc.hpp"
#include "DebugLayer/DebugMacros.hpp"

MortonComputePipeline::MortonComputePipeline(ID3D12Device14 &Device) : FluidComputePipelineBase(Device) {}

MortonComputePipeline::~MortonComputePipeline() = default;

void MortonComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList)
{

    std::vector<MortonSB> SRVParticleData(ParticleCount);
    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        sizeof(MortonSB) * ParticleCount,
        SRVParticleData.data(),
        MortonSRVGPU.DefaultBuffer,
        MortonSRVGPU.UploadBuffer
    );

    std::vector<UINT> UAVParticleData(ParticleCount);
     Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        sizeof(UINT) * ParticleCount,
        UAVParticleData.data(),
        MortonUAVGPU.DefaultBuffer,
        MortonUAVGPU.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );
}

void MortonComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc)
{
    MortonUAVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::UAV, MortonUAVGPU.DefaultBuffer, ParticleCount, sizeof(UINT));

    MortonSRVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::SRV, MortonSRVGPU.DefaultBuffer, ParticleCount, sizeof(MortonSB));
}