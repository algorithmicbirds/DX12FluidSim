#include "FluidPipelines/BuildGridComputePipeline.hpp"
#include "Shared/SimData.hpp"

BuildGridComputePipeline::BuildGridComputePipeline(ID3D12Device14 &Device) : FluidComputePipelineBase(Device) {}

void BuildGridComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    UINT StructuredBufferSize = sizeof(UINT) * SimInitials::HashTableSize;
    std::vector<UINT> Data(SimInitials::HashTableSize, 0xFFFFFFFF);

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
    CellStartUAVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::UAV, CellStartGPUResources.DefaultBuffer, SimInitials::HashTableSize, sizeof(UINT)
    );

    CellStartSRVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::SRV, CellStartGPUResources.DefaultBuffer, SimInitials::HashTableSize, sizeof(UINT)
    );

    CellEndUAVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::UAV, CellEndGPUResources.DefaultBuffer, SimInitials::HashTableSize, sizeof(UINT)
    );

    CellEndSRVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::SRV, CellEndGPUResources.DefaultBuffer, SimInitials::HashTableSize, sizeof(UINT)
    );
}
