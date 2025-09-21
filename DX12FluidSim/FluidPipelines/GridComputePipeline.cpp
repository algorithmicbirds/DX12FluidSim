#include "GridComputePipeline.hpp"
#include "FluidPipelines/FluidPipelinesHeapDesc.hpp"
#include "DebugLayer/DebugMacros.hpp"

GridComputePipeline::GridComputePipeline(ID3D12Device14 &Device) : FluidComputePipelineBase(Device) {}

GridComputePipeline::~GridComputePipeline() = default;

void GridComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList)
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

void GridComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc)
{
    GridUAVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::UAV, GridGPU.DefaultBuffer, ParticleCount, sizeof(GridSB));

    GridSRVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::SRV, GridGPU.DefaultBuffer, ParticleCount, sizeof(GridSB));
}