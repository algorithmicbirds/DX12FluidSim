#pragma once
#include "GlobInclude/WinInclude.hpp"
#include <string>
#include <vector>
#include "Shared/SimData.hpp"

struct ParticleStructuredBuffer
{
    DirectX::XMFLOAT3 Position;
    float ParticleRadius = ParticleInitialValues::ParticleRadius;
    DirectX::XMFLOAT3 Velocity;
    float ParticleSmoothingRadius = ParticleInitialValues::ParticleSmoothingRadius;
    float Density = 0;
    DirectX::XMFLOAT2 Pressure;
    float Mass = 1.0f;
};

struct ParticleGPUData
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> UploadBuffer;
};

struct DebugStructuredBuffer
{
    float DebugDensity;
    float DebugParticleCount;
};

struct GPUDebugResources
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> ReadBackBuffer;
};

class FluidComputePipeline
{
public:
    FluidComputePipeline(ID3D12Device14 &Device);
    ~FluidComputePipeline();

    void CreatePipeline(const std::string &CSFilePath, class FluidHeapDescriptor &HeapDesc);
    void CreateBufferDesc(class FluidHeapDescriptor &HeapDesc);
    void BindRootAndPSO(ID3D12GraphicsCommandList7 *CmdList);

    void SetRootSignature(ComPtr<ID3D12RootSignature> InRootSig) { RootSignature = InRootSig; }
    void CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList, UINT Count);
    void ReadDebugBuffer(ID3D12GraphicsCommandList7 *CmdList);
    void CreateDensityTexture();
    void ArrangeParticlesRandomly(std::vector<ParticleStructuredBuffer> &particleData);
    void ArrangeParticlesInSquare(std::vector<ParticleStructuredBuffer> &particleData);
    D3D12_GPU_VIRTUAL_ADDRESS GetDensityTexGPUAddress() const
    {
        return DensityTexture->GetGPUVirtualAddress();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GetParticleForcesSRVGPUHandle() const { return ParticleForcesSRVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetParticleForcesUAVGPUHandle() const { return ParticleForcesUAVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetDebugUAVGPUHandle() const { return DebugUAVGPUHandle; }
    ID3D12Resource2 *GetParticleBuffer() { return ParticleData.DefaultBuffer.Get(); }

private:
    void CreatePipelineState(const std::vector<char> &CSCode);

private:
    ID3D12Device14 &DeviceRef;

    UINT ParticleCount = 0;

    ComPtr<ID3D12RootSignature> RootSignature;
    ComPtr<ID3D12PipelineState> PipelineState;
    ComPtr<ID3D12Resource2> DensityTexture;

    D3D12_GPU_DESCRIPTOR_HANDLE ParticleForcesUAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE ParticleForcesSRVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE DebugUAVGPUHandle{};

    ParticleGPUData ParticleData;
    GPUDebugResources GPUDebugResourcesData;
};
