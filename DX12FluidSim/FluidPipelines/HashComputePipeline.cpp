#include "FluidPipelines/HashComputePipeline.hpp"
#include "FluidPipelines/FluidPipelinesHeapDesc.hpp"
#include "DebugLayer/DebugMacros.hpp"

HashComputePipeline::HashComputePipeline(ID3D12Device14 &Device) : FluidComputePipelineBase(Device) {}

HashComputePipeline::~HashComputePipeline() = default;

void HashComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    UINT StructuredBufferSize = sizeof(HashData) * ParticleCount;
    std::vector<HashData> particleData(ParticleCount);

    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        StructuredBufferSize,
        particleData.data(),
        HashGPU.DefaultBuffer,
        HashGPU.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );
}

void HashComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc)
{
    HashUAVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::UAV, HashGPU.DefaultBuffer, ParticleCount, sizeof(HashData));

    HashSRVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::SRV, HashGPU.DefaultBuffer, ParticleCount, sizeof(HashData));
}