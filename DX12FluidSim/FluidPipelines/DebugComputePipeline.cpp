#include "DebugComputePipeline.hpp"

DebugComputePipeline::DebugComputePipeline(ID3D12Device14 &Device) : FluidComputePipelineBase(Device) {}

void DebugComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList) {
    UINT StructuredBufferSize = sizeof(HashData) * SimInitials::ParticleCount;
    std::vector<HashData> Data(SimInitials::ParticleCount);

    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        StructuredBufferSize,
        Data.data(),
        DebugGPU.DefaultBuffer,
        DebugGPU.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );
}

void DebugComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc) {}
