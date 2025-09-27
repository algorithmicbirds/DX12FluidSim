#include "FluidIntegrateComputePipeline.hpp"
#include "Shared/Utils.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "FluidPipelines/FluidPipelinesHeapDesc.hpp"

FluidIntegrateComputePipeline::FluidIntegrateComputePipeline(ID3D12Device14 &Device) : FluidComputePipelineBase(Device) {}

FluidIntegrateComputePipeline::~FluidIntegrateComputePipeline() = default;

void FluidIntegrateComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    UINT StructuredBufferSize = sizeof(ParticleStructuredBuffer) * ParticleCount;
    std::vector<ParticleStructuredBuffer> Particles(ParticleCount);

    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        StructuredBufferSize,
        Particles.data(),
        ParticleGPU.DefaultBuffer,
        ParticleGPU.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );
}

void FluidIntegrateComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc)
{
    ParticleIntegrateUAVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::UAV, ParticleGPU.DefaultBuffer, ParticleCount, sizeof(ParticleStructuredBuffer)
    );

    ParticleIntegrateSRVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::SRV, ParticleGPU.DefaultBuffer, ParticleCount, sizeof(ParticleStructuredBuffer)
    );
}