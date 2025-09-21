#include "FluidForcesComputePipeline.hpp"
#include "Shared/Utils.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include <random>
#include <iostream>
#include "FluidPipelines/FluidPipelinesHeapDesc.hpp"

FluidForcesComputePipeline::FluidForcesComputePipeline(ID3D12Device14 &Device) : FluidComputePipelineBase(Device) {}

FluidForcesComputePipeline::~FluidForcesComputePipeline() {}

void FluidForcesComputePipeline::CreateBufferDesc(FluidHeapDescriptor &HeapDesc)
{
    ParticleForcesUAVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::UAV, ParticleData.DefaultBuffer, ParticleCount, sizeof(ParticleStructuredBuffer)
    );

    ParticleForcesSRVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::SRV, ParticleData.DefaultBuffer, ParticleCount, sizeof(ParticleStructuredBuffer)
    );
}

void FluidForcesComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList)
{
    UINT StructuredBufferSize = sizeof(ParticleStructuredBuffer) * ParticleCount;
    std::vector<ParticleStructuredBuffer> particleData(ParticleCount);

    // ArrangeParticlesInSquare(particleData);
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
        Particle.Acceleration = {0.0f, 0.0f, 0.0f};
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