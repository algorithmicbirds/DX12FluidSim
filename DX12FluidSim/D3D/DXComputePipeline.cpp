#include "DXComputePipeline.hpp"
#include "Shared/Utils.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include <random>
#include <iostream>

DXComputePipeline::DXComputePipeline(ID3D12Device14 &Device) : DeviceRef(Device) {}

DXComputePipeline::~DXComputePipeline() {}

void DXComputePipeline::CreatePipeline(const std::string &CSFilePath)
{
    std::vector<char> CSCode = Utils::ReadFile(CSFilePath);
    CreateDensityTexture();
    CreateDescHeap();
    CreatePipelineState(CSCode);
}

void DXComputePipeline::CreateDescHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC HeapDesc{};
    HeapDesc.NumDescriptors = 6;
    HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    DX_VALIDATE(DeviceRef.CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&DescriptorHeap)), DescriptorHeap);
    ParticleUAVGPUHandle = Utils::CreateBufferDescriptor(
        DeviceRef,
        DescriptorType::UAV,
        ParticleData.DefaultBuffer,
        DescriptorHeap,
        ParticleCount,
        sizeof(ParticleStructuredBuffer),
        0
    );

    ParticleSRVGPUHandle = Utils::CreateBufferDescriptor(
        DeviceRef,
        DescriptorType::SRV,
        ParticleData.DefaultBuffer,
        DescriptorHeap,
        ParticleCount,
        sizeof(ParticleStructuredBuffer),
        1
    );

    DebugUAVGPUHandle = Utils::CreateBufferDescriptor(
        DeviceRef,
        DescriptorType::UAV,
        GPUDebugResourcesData.DefaultBuffer,
        DescriptorHeap,
        ParticleCount,
        sizeof(DebugStructuredBuffer),
        2
    );
}

void DXComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList, UINT Count)
{
    ParticleCount = Count;
    UINT StructuredBufferSize = sizeof(ParticleStructuredBuffer) * ParticleCount;
    std::vector<ParticleStructuredBuffer> particleData(ParticleCount);

    //ArrangeParticlesInSquare(particleData);
    ArrangeParticlesRandomly(particleData);

    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        StructuredBufferSize,
        particleData.data(),
        ParticleData.DefaultBuffer,
        ParticleData.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );

    UINT DebugBufSize = sizeof(DebugStructuredBuffer) * ParticleCount;

    GPUDebugResourcesData.DefaultBuffer = Utils::CreateBuffer(
        DeviceRef,
        DebugBufSize,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );

    GPUDebugResourcesData.ReadBackBuffer =
        Utils::CreateBuffer(DeviceRef, DebugBufSize, D3D12_HEAP_TYPE_READBACK, D3D12_RESOURCE_STATE_COPY_DEST);
}

void DXComputePipeline::ReadDebugBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    if (!GPUDebugResourcesData.DefaultBuffer || !GPUDebugResourcesData.ReadBackBuffer)
        return;

    Utils::TransitionResoure(
        CmdList,
        GPUDebugResourcesData.DefaultBuffer.Get(),
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        D3D12_RESOURCE_STATE_COPY_SOURCE
    );

    CmdList->CopyResource(GPUDebugResourcesData.ReadBackBuffer.Get(), GPUDebugResourcesData.DefaultBuffer.Get());

    Utils::TransitionResoure(
        CmdList,
        GPUDebugResourcesData.DefaultBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_SOURCE,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS
    );

    DebugStructuredBuffer *mappedData = nullptr;
    D3D12_RANGE readRange{0, sizeof(DebugStructuredBuffer) * ParticleCount};
    GPUDebugResourcesData.ReadBackBuffer->Map(0, &readRange, reinterpret_cast<void **>(&mappedData));

    for (UINT i = 0; i < ParticleCount; ++i)
    {
        std::cout << "Particle " << i << ": Density = " << mappedData[i].DebugDensity
                  << ", ParticleCount = " << mappedData[i].DebugParticleCount << "\n";
    }

    D3D12_RANGE writtenRange{0, 0};
    GPUDebugResourcesData.ReadBackBuffer->Unmap(0, &writtenRange);
}

void DXComputePipeline::CreateDensityTexture()
{
    D3D12_RESOURCE_DESC TexDesc{};
    TexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    TexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    TexDesc.Format = DXGI_FORMAT_R32_FLOAT;
    TexDesc.Width = 1920;
    TexDesc.Height = 1080;
    TexDesc.MipLevels = 1;
    TexDesc.DepthOrArraySize = 1;
    TexDesc.SampleDesc.Count = 1;
    TexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    
    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    DX_VALIDATE(
        DeviceRef.CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &TexDesc,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            nullptr,
            IID_PPV_ARGS(&DensityTexture)
        ),
        DensityTexture
    );
}

void DXComputePipeline::ArrangeParticlesRandomly(std::vector<ParticleStructuredBuffer> &particleData)
{
    const float boxWidth = 6.0f;
    const float boxHeight = 3.5f;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(-boxWidth / 2.0f, boxWidth / 2.0f);
    std::uniform_real_distribution<float> distY(-boxHeight / 2.0f, boxHeight / 2.0f);

    for (UINT i = 0; i < ParticleCount; ++i)
    {
        ParticleStructuredBuffer &p = particleData[i];

        p.Position.x = distX(gen);
        p.Position.y = distY(gen);
        p.Position.z = 0.0f;

        p.Velocity = {0.0f, 0.0f, 0.0f};
    }
}

void DXComputePipeline::ArrangeParticlesInSquare(std::vector<ParticleStructuredBuffer> &particleData)
{
    int particlesPerRow = (int)sqrt(ParticleCount);
    int particlesPerCol = (ParticleCount - 1) / particlesPerRow + 1;

    float particleSpacing = 0.05f;

    float gridWidth = particlesPerRow * (2 * particleData[0].ParticleRadius + particleSpacing);
    float gridHeight = particlesPerCol * (2 * particleData[0].ParticleRadius + particleSpacing);

    for (UINT i = 0; i < ParticleCount; ++i)
    {
        ParticleStructuredBuffer &p = particleData[i];
        int row = i / particlesPerRow;
        int col = i % particlesPerRow;

        float spacing = p.ParticleRadius * 2 + particleSpacing;

        p.Position.x = col * spacing - gridWidth / 2 + p.ParticleRadius;
        p.Position.y = row * spacing - gridHeight / 2 + p.ParticleRadius;
        p.Position.z = 0.0f;

        p.Velocity = {0.0f, 0.0f, 0.0f};
    }
}

void DXComputePipeline::CreatePipelineState(const std::vector<char> &CSCode)
{
    VALIDATE_PTR(RootSignature.Get());

    D3D12_COMPUTE_PIPELINE_STATE_DESC Desc{};
    Desc.pRootSignature = RootSignature.Get();
    Desc.CS.BytecodeLength = CSCode.size();
    Desc.CS.pShaderBytecode = CSCode.data();

    DX_VALIDATE(DeviceRef.CreateComputePipelineState(&Desc, IID_PPV_ARGS(&PipelineState)), PipelineState);
}

void DXComputePipeline::BindRootAndPSO(ID3D12GraphicsCommandList7 *CmdList)
{
    CmdList->SetPipelineState(PipelineState.Get());
    CmdList->SetComputeRootSignature(RootSignature.Get());
}
