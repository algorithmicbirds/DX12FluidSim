#pragma once
#include "GlobInclude/WinInclude.hpp"
#include <string>
#include <vector>

struct Particle
{
    DirectX::XMFLOAT3 Position;
    float ParticleRadius = 0.1f;
    DirectX::XMFLOAT3 Velocity; 
    float Pad1;
};

struct ParticleGPUData
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> UploadBuffer;
};

class DXComputePipeline
{
public:
    DXComputePipeline(ID3D12Device14 &Device);
    ~DXComputePipeline();

    void CreatePipeline(const std::string &CSFilePath);
    void Dispatch(ID3D12GraphicsCommandList7 *CmdList);

    void SetRootSignature(ComPtr<ID3D12RootSignature> InRootSig) { RootSignature = InRootSig; }
    void CreateStructuredBuffer(ID3D12GraphicsCommandList7* CmdList, UINT Count);
    
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() const { return SRVGPUHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetUAVGPUHandle() const { return UAVGPUHandle; }
    ID3D12Resource2 *GetParticleBuffer() { return ParticleData.DefaultBuffer.Get(); }
    ID3D12DescriptorHeap *GetDescriptorHeap() { return DescriptorHeap.Get(); }


private:
    void CreateDescHeap();
    void CreateUAVDesc();
    void CreateSRVDesc();
    void CreatePipelineState(const std::vector<char> &CSCode);

private:
    ID3D12Device14 &DeviceRef;

    UINT ParticleCount = 0;

    ComPtr<ID3D12DescriptorHeap> DescriptorHeap;

    ComPtr<ID3D12RootSignature> RootSignature;
    ComPtr<ID3D12PipelineState> PipelineState;

    D3D12_GPU_DESCRIPTOR_HANDLE UAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE SRVGPUHandle{};

    ParticleGPUData ParticleData;
};
