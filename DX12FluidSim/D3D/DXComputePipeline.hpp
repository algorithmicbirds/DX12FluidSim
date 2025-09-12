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
    float Pressure;
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

class DXComputePipeline
{
public:
    DXComputePipeline(ID3D12Device14 &Device);
    ~DXComputePipeline();

    void CreatePipeline(const std::string &CSFilePath);
    void BindRootAndPSO(ID3D12GraphicsCommandList7 *CmdList);

    void SetRootSignature(ComPtr<ID3D12RootSignature> InRootSig) { RootSignature = InRootSig; }
    void CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList, UINT Count);
    void ReadDebugBuffer(ID3D12GraphicsCommandList7 *CmdList);
    void CreateDensityTexture();
    void ArrangeParticlesInSquare(std::vector<ParticleStructuredBuffer> &particleData);
    
    D3D12_GPU_DESCRIPTOR_HANDLE GetParticleSRVGPUHandle() const { return ParticleSRVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetParticleUAVGPUHandle() const { return ParticleUAVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetDebugUAVGPUHandle() const { return DebugUAVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetDensityTexUAVGPUHandle() const { return DensityTexUAVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetDesnsityTexSRVGPUHandle() const { return DensityTexSRVGPUHandle; }
    ID3D12Resource2 *GetParticleBuffer() { return ParticleData.DefaultBuffer.Get(); }
    ID3D12DescriptorHeap *GetDescriptorHeap() { return DescriptorHeap.Get(); }


private:
    void CreateDescHeap();
    void CreatePipelineState(const std::vector<char> &CSCode);

private:
    ID3D12Device14 &DeviceRef;

    UINT ParticleCount = 0;

    ComPtr<ID3D12DescriptorHeap> DescriptorHeap;

    ComPtr<ID3D12RootSignature> RootSignature;
    ComPtr<ID3D12PipelineState> PipelineState;
    ComPtr<ID3D12Resource2> DensityTexture;

    D3D12_GPU_DESCRIPTOR_HANDLE ParticleUAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE ParticleSRVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE DebugUAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE DensityTexUAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE DensityTexSRVGPUHandle{};

    ParticleGPUData ParticleData;
    GPUDebugResources GPUDebugResourcesData;
};
