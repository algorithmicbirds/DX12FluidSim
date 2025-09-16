#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Shared/SimData.hpp"

#include <vector>
#include <string>

struct ParticleIntegrateGD
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> UploadBuffer;
};

struct ParticleIntegrateSB
{
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT3 Velocity;
};

class FluidIntegrateComputePipeline
{
public:
    FluidIntegrateComputePipeline(ID3D12Device14 &Device);
    ~FluidIntegrateComputePipeline();

    FluidIntegrateComputePipeline &operator=(const FluidIntegrateComputePipeline &) = delete;
    FluidIntegrateComputePipeline(const FluidIntegrateComputePipeline &) = delete;

    void CreatePipeline(const std::string &CSFilePath, class FluidHeapDescriptor &HeapDesc);
    void BindRootAndPSO(ID3D12GraphicsCommandList7 *CmdList);
    void SetRootSignature(ComPtr<ID3D12RootSignature> InRootSig) { RootSignature = InRootSig; }
    void CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList);

private:
    void CreatePipelineState(const std::vector<char> &CSCode);
    void CreateBufferDesc(class FluidHeapDescriptor & HeapDesc);

private:
    ComPtr<ID3D12RootSignature> RootSignature;
    ComPtr<ID3D12PipelineState> PipelineStateObject;
    ParticleIntegrateGD ParticleGPU;

    ID3D12Device14 &DeviceRef;

    D3D12_GPU_DESCRIPTOR_HANDLE ParticleIntegrateUAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE ParticleIntegrateSRVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE DebugUAVGPUHandle{};

    UINT ParticleCount = SimInitials::ParticleCount;
};