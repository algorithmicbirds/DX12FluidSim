#pragma once
#include "GlobInclude/WinInclude.hpp"
#include <string>
#include <vector>
#include "Shared/SimData.hpp"
#include "FluidPipelines/FluidComputePipelineBase.hpp"

struct ParticleStructuredBuffer
{
    DirectX::XMFLOAT3 Position;
    float ParticleRadius = ParticleInitialValues::ParticleRadius;
    DirectX::XMFLOAT3 Acceleration;
    float ParticleSmoothingRadius = ParticleInitialValues::ParticleSmoothingRadius;
    DirectX::XMFLOAT3 Velocity;
    float Density = 0;
    DirectX::XMFLOAT2 Pressure;
    float Mass = 1.0f;
};

struct ParticleGPUData
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> UploadBuffer;
};

class FluidForcesComputePipeline : public FluidComputePipelineBase
{
public:
    FluidForcesComputePipeline(ID3D12Device14 &Device);
    ~FluidForcesComputePipeline();

    FluidForcesComputePipeline &operator=(const FluidForcesComputePipeline &) = delete;
    FluidForcesComputePipeline(const FluidForcesComputePipeline &) = delete;

    void CreateBufferDesc(FluidHeapDescriptor &HeapDesc) override;
    void CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList) override;

    void ArrangeParticlesRandomly(std::vector<ParticleStructuredBuffer> &particleData);
    void ArrangeParticlesInSquare(std::vector<ParticleStructuredBuffer> &particleData);

    D3D12_GPU_DESCRIPTOR_HANDLE GetParticleForcesSRVGPUHandle() const { return ParticleForcesSRVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetParticleForcesUAVGPUHandle() const { return ParticleForcesUAVGPUHandle; }
    ID3D12Resource2 *GetParticleForcesBuffer() const { return ParticleData.DefaultBuffer.Get(); }

private:
    const UINT ParticleCount = SimInitials::ParticleCount;

    ComPtr<ID3D12RootSignature> RootSignature;
    ComPtr<ID3D12PipelineState> PipelineState;

    D3D12_GPU_DESCRIPTOR_HANDLE ParticleForcesUAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE ParticleForcesSRVGPUHandle{};

    ParticleGPUData ParticleData;
};
