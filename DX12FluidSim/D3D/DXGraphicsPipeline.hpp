#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>
#include <string>

class DXGraphicsPipeline
{
public:
    DXGraphicsPipeline(ID3D12Device14 &Device);
    ~DXGraphicsPipeline();
    DXGraphicsPipeline &operator=(const DXGraphicsPipeline &) = delete;
    DXGraphicsPipeline(const DXGraphicsPipeline &) = delete;

public:
    void SetRootSignature(ComPtr<ID3D12RootSignature> InRootSignature) { RootSignature = InRootSignature; }
    void CreatePipeline(const std::string &VertexShaderPath, const std::string &PixelShaderPath);
    void Dispatch(ID3D12GraphicsCommandList7 *CmdList);

private:
    bool Init();
    void CreatePipelineState(std::vector<char> &VertexShaderCode, std::vector<char> &PixelShaderCode);

    // Initializers
    D3D12_RASTERIZER_DESC InitRasterizerDesc();
    D3D12_STREAM_OUTPUT_DESC InitStreamOutputDesc();
    D3D12_BLEND_DESC InitBlendDesc();
    D3D12_DEPTH_STENCIL_DESC InitDepthStencilDesc();

private:
    ID3D12Device14 &DeviceRef;
    ComPtr<ID3D12PipelineState> PipelineStateObject;
    ComPtr<ID3D12RootSignature> RootSignature;
};