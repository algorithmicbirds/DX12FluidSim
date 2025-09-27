#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Shared/SimData.hpp"
#include "FluidPipelines/FluidComputePipelineBase.hpp"

#include <vector>
#include <string>

struct ParticleIntegrateGPU
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> UploadBuffer;
};

class FluidIntegrateComputePipeline : public FluidComputePipelineBase
{
public:
    FluidIntegrateComputePipeline(ID3D12Device14 &Device);
    ~FluidIntegrateComputePipeline() override;

    FluidIntegrateComputePipeline &operator=(const FluidIntegrateComputePipeline &) = delete;
    FluidIntegrateComputePipeline(const FluidIntegrateComputePipeline &) = delete;

    void SetRootSignature(ComPtr<ID3D12RootSignature> InRootSig) { RootSignature = InRootSig; }
    void CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList) override;

    D3D12_GPU_DESCRIPTOR_HANDLE GetParticleIntegrateSRVGPUHandle() const { return ParticleIntegrateSRVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetParticleIntegrateUAVGPUHandle() const { return ParticleIntegrateUAVGPUHandle; }
    ID3D12Resource2* GetParticleIntegrateBuffer() const { return ParticleGPU.DefaultBuffer.Get(); }

private:
    void CreatePipelineState(const std::vector<char> &CSCode);
    void CreateBufferDesc(class FluidHeapDescriptor &HeapDesc) override;

private:
    ParticleIntegrateGPU ParticleGPU;

    D3D12_GPU_DESCRIPTOR_HANDLE ParticleIntegrateUAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE ParticleIntegrateSRVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE DebugUAVGPUHandle{};

    UINT ParticleCount = SimInitials::ParticleCount;
};