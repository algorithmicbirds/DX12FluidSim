#include "Renderer/StupidDebugBuffer.hpp"
#include "Shared/Utils.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include <iostream>

void StupidDebugBuffer::ReadBackDebugBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    Utils::TransitionResoure(
        CmdList, GPUDebug.DefaultBuffer.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE
    );

    CmdList->CopyResource(GPUDebug.ReadBackBuffer.Get(), GPUDebug.DefaultBuffer.Get());

    Utils::TransitionResoure(
        CmdList, GPUDebug.DefaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS
    );

    void *pData = nullptr;
    D3D12_RANGE readRange{0, sizeof(PixelDebugStructuredBuffer) * ParticleCount};
    GPUDebug.ReadBackBuffer->Map(0, &readRange, &pData);

    PixelDebugStructuredBuffer *debugData = reinterpret_cast<PixelDebugStructuredBuffer *>(pData);
    for (UINT i = 0; i < ParticleCount; ++i)
    {

        std::cout << "Particle " << i << " density = " << debugData[i].Density << "\n ";
    }

    GPUDebug.ReadBackBuffer->Unmap(0, nullptr);
}

void StupidDebugBuffer::CreateDebugUAVAndDesc(ID3D12Device14 &Device)
{
    UINT DebugBufSize = sizeof(PixelDebugStructuredBuffer) * ParticleCount;
    GPUDebug.DefaultBuffer = Utils::CreateBuffer(
        Device,
        DebugBufSize,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );

    GPUDebug.ReadBackBuffer =
        Utils::CreateBuffer(Device, DebugBufSize, D3D12_HEAP_TYPE_READBACK, D3D12_RESOURCE_STATE_COPY_DEST);

    D3D12_DESCRIPTOR_HEAP_DESC HeapDesc{};
    HeapDesc.NumDescriptors = 1;
    HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    DX_VALIDATE(Device.CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&DebugDescHeap)), DebugDescHeap);

    DebugGPUDescHandle = Utils::CreateBufferDescriptor(
        Device,
        DescriptorType::UAV,
        GPUDebug.DefaultBuffer,
        DebugDescHeap,
        ParticleCount,
        sizeof(PixelDebugStructuredBuffer),
        0
    );
}
