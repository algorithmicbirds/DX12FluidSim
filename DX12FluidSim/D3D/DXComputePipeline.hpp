#pragma once
#include "GlobInclude/WinInclude.hpp"
#include <string>
#include <vector>

class DXComputePipeline
{
public:
    DXComputePipeline(ID3D12Device14 &Device);
    ~DXComputePipeline();

    void CreatePipeline(const std::string &CSFilePath, UINT Width, UINT Height, UINT UAVIndexIn);
    void Dispatch(ID3D12GraphicsCommandList7 *CmdList);

    D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() const { return SRVGPUHandle; }
    ID3D12Resource2 *GetOutputTexture() const { return OutputTexture.Get(); }
    void SetRootSignature(ComPtr<ID3D12RootSignature> InRootSig) { RootSignature = InRootSig; }

private:
    void CreateTexture();
    void CreateDescHeap();
    void CreateUAVDesc();
    void CreateSRVDesc();
    void CreatePipelineState(const std::vector<char> &CSCode);

private:
    ID3D12Device14 &DeviceRef;

    UINT TexWidth = 0;
    UINT TexHeight = 0;
    UINT UAVIndex = 0;

    ComPtr<ID3D12Resource2> OutputTexture;
    ComPtr<ID3D12DescriptorHeap> DescriptorHeap;

    ComPtr<ID3D12RootSignature> RootSignature;
    ComPtr<ID3D12PipelineState> PipelineState;

    D3D12_GPU_DESCRIPTOR_HANDLE UAVGPUHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE SRVGPUHandle{};
};
