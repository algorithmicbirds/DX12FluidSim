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
    HashGPUResources.DefaultBuffer = Utils::CreateBuffer(
        DeviceRef,
        StructuredBufferSize,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );

    HashGPUResources.ReadBackBuffer =
        Utils::CreateBuffer(DeviceRef, StructuredBufferSize, D3D12_HEAP_TYPE_READBACK, D3D12_RESOURCE_STATE_COPY_DEST);
    HashGPUResources.UploadBuffer =
        Utils::CreateBuffer(DeviceRef, StructuredBufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void HashComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc)
{
    HashUAVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::UAV, HashGPUResources.DefaultBuffer, ParticleCount, sizeof(HashDataSB)
    );
}

void HashComputePipeline::ReadSortUpdateHashBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    ReadBackHashBuffer(CmdList);
    SortHashData();
    WriteSortedHashesToBuffer();
    UploadSortedHashesToGPU(CmdList);
}

void HashComputePipeline::ReadBackHashBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    Utils::TransitionResoure(
        CmdList,
        HashGPUResources.DefaultBuffer.Get(),
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        D3D12_RESOURCE_STATE_COPY_SOURCE
    );
    CmdList->CopyResource(HashGPUResources.ReadBackBuffer.Get(), HashGPUResources.DefaultBuffer.Get());
    Utils::TransitionResoure(
        CmdList,
        HashGPUResources.DefaultBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_SOURCE,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS
    );

    void *pData = nullptr;
    D3D12_RANGE readRange{0, sizeof(HashDataSB) * ParticleCount};
    HashGPUResources.ReadBackBuffer->Map(0, &readRange, &pData);
    HashSBCPU.clear();
    HashDataSB *hashData = reinterpret_cast<HashDataSB *>(pData);
    for (UINT i = 0; i < ParticleCount; i++)
    {
        HashSBCPU.push_back(hashData[i]);
    }

    HashGPUResources.ReadBackBuffer->Unmap(0, nullptr);
}

void HashComputePipeline::SortHashData()
{
    std::sort(
        HashSBCPU.begin(),
        HashSBCPU.end(),
        [](const HashDataSB &a, const HashDataSB &b) { return a.HashCode < b.HashCode; }
    );
}

void HashComputePipeline::WriteSortedHashesToBuffer()
{
    void *pData = nullptr;
    D3D12_RANGE writeRange{0, sizeof(HashDataSB) * ParticleCount};
    HashGPUResources.UploadBuffer->Map(0, &writeRange, &pData);
    HashDataSB *sortedHashData = reinterpret_cast<HashDataSB *>(pData);
    for (UINT i = 0; i < ParticleCount; ++i)
    {
        sortedHashData[i] = HashSBCPU[i];
    }
    HashGPUResources.UploadBuffer->Unmap(0, nullptr);
}

void HashComputePipeline::UploadSortedHashesToGPU(ID3D12GraphicsCommandList7 *CmdList) {
    Utils::TransitionResoure(
        CmdList,
        HashGPUResources.UploadBuffer.Get(),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        D3D12_RESOURCE_STATE_COPY_SOURCE
    );
    Utils::TransitionResoure(
        CmdList,
        HashGPUResources.DefaultBuffer.Get(),
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        D3D12_RESOURCE_STATE_COPY_DEST
    );
    CmdList->CopyResource(HashGPUResources.DefaultBuffer.Get(), HashGPUResources.UploadBuffer.Get());
    Utils::TransitionResoure(
        CmdList,
        HashGPUResources.DefaultBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS
    );
}
