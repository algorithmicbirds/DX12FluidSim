#include "FluidPipelines/HashComputePipeline.hpp"
#include "FluidPipelines/FluidPipelinesHeapDesc.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include <iostream>
#include <algorithm>

HashComputePipeline::HashComputePipeline(ID3D12Device14 &Device) : FluidComputePipelineBase(Device) {}

HashComputePipeline::~HashComputePipeline() = default;

void HashComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    UINT StructuredBufferSize = sizeof(HashDataSB) * ParticleCount;
    HashSBCPU.reserve(ParticleCount);
    HashGPU.DefaultBuffer = Utils::CreateBuffer(
        DeviceRef,
        StructuredBufferSize,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );

    HashGPU.ReadBackBuffer =
        Utils::CreateBuffer(DeviceRef, StructuredBufferSize, D3D12_HEAP_TYPE_READBACK, D3D12_RESOURCE_STATE_COPY_DEST);
}

void HashComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc)
{
    HashUAVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::UAV, HashGPU.DefaultBuffer, ParticleCount, sizeof(HashDataSB));

    HashSRVGPUHandle =
        HeapDesc.AllocateDescriptor(DescriptorType::SRV, HashGPU.DefaultBuffer, ParticleCount, sizeof(HashDataSB));
}

void HashComputePipeline::ReadBackHashBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    Utils::TransitionResoure(
        CmdList, HashGPU.DefaultBuffer.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE
    );
    CmdList->CopyResource(HashGPU.ReadBackBuffer.Get(), HashGPU.DefaultBuffer.Get());
    Utils::TransitionResoure(
        CmdList, HashGPU.DefaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS
    );

    void *pData = nullptr;
    D3D12_RANGE readRange{0, sizeof(HashDataSB) * ParticleCount};
    HashGPU.ReadBackBuffer->Map(0, &readRange, &pData);
    HashSBCPU.clear();
    HashDataSB *hashData = reinterpret_cast<HashDataSB *>(pData);
    for (UINT i = 0; i < ParticleCount; i++)
    {
        HashSBCPU.push_back(hashData[i]);
    }

    HashGPU.ReadBackBuffer->Unmap(0, nullptr);
}

void HashComputePipeline::SortHashedValues()
{
    std::sort(
        HashSBCPU.begin(),
        HashSBCPU.end(),
        [](const HashDataSB &a, const HashDataSB &b) { return a.HashCode < b.HashCode; }
    );
}
