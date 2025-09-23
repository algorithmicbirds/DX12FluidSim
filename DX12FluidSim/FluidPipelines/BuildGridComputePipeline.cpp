#include "FluidPipelines/BuildGridComputePipeline.hpp"
#include "Shared/SimData.hpp"

BuildGridComputePipeline::BuildGridComputePipeline(ID3D12Device14 &Device) : FluidComputePipelineBase(Device) {}

void BuildGridComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    UINT StructuredBufferSize = sizeof(UINT) * SimInitials::ParticleCount;
    std::vector<UINT> Data(SimInitials::ParticleCount);

    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        StructuredBufferSize,
        Data.data(),
        CellStartGPUResources.DefaultBuffer,
        CellStartGPUResources.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );

    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        StructuredBufferSize,
        Data.data(),
        CellEndGPUResources.DefaultBuffer,
        CellEndGPUResources.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );
}

void BuildGridComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc)
{
    CellStartSRVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::UAV, CellStartGPUResources.DefaultBuffer, SimInitials::ParticleCount, sizeof(UINT)
    );

    CellStartSRVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::SRV, CellStartGPUResources.DefaultBuffer, SimInitials::ParticleCount, sizeof(UINT)
    );

    CellEndUAVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::UAV, CellEndGPUResources.DefaultBuffer, SimInitials::ParticleCount, sizeof(UINT)
    );

    CellEndSRVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::SRV, CellEndGPUResources.DefaultBuffer, SimInitials::ParticleCount, sizeof(UINT)
    );
}
