#include "FluidPipelines/MortonComputePipeline.hpp"
#include "FluidPipelines/FluidPipelinesHeapDesc.hpp"
#include "DebugLayer/DebugMacros.hpp"

MortonComputePipeline::MortonComputePipeline(ID3D12Device14 &Device) : FluidComputePipelineBase(Device) {}

MortonComputePipeline::~MortonComputePipeline() = default;

void MortonComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    UINT StructuredBufferSize = sizeof(GridSB) * ParticleCount;
    std::vector<GridSB> particleData(ParticleCount);

    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        StructuredBufferSize,
        particleData.data(),
        GridGPU.DefaultBuffer,
        GridGPU.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );
}

void MortonComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc)
{
    MortonUAVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::UAV, GridGPU.DefaultBuffer, ParticleCount, sizeof(GridSB));

    MortonSRVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::SRV, GridGPU.DefaultBuffer, ParticleCount, sizeof(GridSB));
}