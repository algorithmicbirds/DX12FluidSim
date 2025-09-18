#include "FluidForcesComputePipeline.hpp"
#include "Shared/Utils.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include <random>
#include <iostream>
#include "FluidPipelines/FluidPipelinesHeapDesc.hpp"

FluidForcesComputePipeline::FluidForcesComputePipeline(ID3D12Device14 &Device) : DeviceRef(Device) {}

FluidForcesComputePipeline::~FluidForcesComputePipeline() {}

void FluidForcesComputePipeline::CreatePipeline(const std::string &CSFilePath, FluidHeapDescriptor &HeapDesc)
{
    std::vector<char> CSCode = Utils::ReadFile(CSFilePath);
    CreateBufferDesc(HeapDesc);
    CreatePipelineState(CSCode);
}

void FluidForcesComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc)
{
    ParticleForcesUAVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::UAV, ParticleData.DefaultBuffer, ParticleCount, sizeof(ParticleStructuredBuffer)
    );

    ParticleForcesSRVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::SRV, ParticleData.DefaultBuffer, ParticleCount, sizeof(ParticleStructuredBuffer)
    );

    DebugUAVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::UAV, GPUDebugResourcesData.DefaultBuffer, ParticleCount, sizeof(DebugStructuredBuffer)
    );
}

void FluidForcesComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList, UINT Count)
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

void FluidForcesComputePipeline::ReadDebugBuffer(ID3D12GraphicsCommandList7 *CmdList)
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

void FluidForcesComputePipeline::CreateDensityTexture()
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

void FluidForcesComputePipeline::ArrangeParticlesRandomly(std::vector<ParticleStructuredBuffer> &particleData)
{
    const float boxWidth = 6.0f;
    const float boxHeight = 3.5f;
    float fixedDelta = 0.008;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(-boxWidth / 2.0f, boxWidth / 2.0f);
    std::uniform_real_distribution<float> distY(-boxHeight / 2.0f, boxHeight / 2.0f);

    for (UINT i = 0; i < ParticleCount; ++i)
    {
        ParticleStructuredBuffer &Particle = particleData[i];

        Particle.Position.x = distX(gen);
        Particle.Position.y = distY(gen);
        Particle.Position.z = 0.0f;

        Particle.Velocity = {0.0f, 0.0f, 0.0f};

        // initialize previous position so verlet doesnt fuck up first dispalcement
        // x{t-Δt} = xt - vt * Δt
        Particle.PreviousPosition.x = Particle.Position.x - Particle.Velocity.x * fixedDelta;
        Particle.PreviousPosition.y = Particle.Position.y - Particle.Velocity.y * fixedDelta;
        Particle.PreviousPosition.z = Particle.Position.z - Particle.Velocity.z * fixedDelta;
    }
}

void FluidForcesComputePipeline::ArrangeParticlesInSquare(std::vector<ParticleStructuredBuffer> &particleData)
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

void FluidForcesComputePipeline::CreatePipelineState(const std::vector<char> &CSCode)
{
    VALIDATE_PTR(RootSignature.Get());

    D3D12_COMPUTE_PIPELINE_STATE_DESC Desc{};
    Desc.pRootSignature = RootSignature.Get();
    Desc.CS.BytecodeLength = CSCode.size();
    Desc.CS.pShaderBytecode = CSCode.data();

    DX_VALIDATE(DeviceRef.CreateComputePipelineState(&Desc, IID_PPV_ARGS(&PipelineState)), PipelineState);
}

void FluidForcesComputePipeline::BindRootAndPSO(ID3D12GraphicsCommandList7 *CmdList)
{
    CmdList->SetPipelineState(PipelineState.Get());
    CmdList->SetComputeRootSignature(RootSignature.Get());
}
