#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>
#include <string>

class DXPipeline
{
public:
    DXPipeline(ID3D12Device14 &Device, const std::string &VertexShaderFilePath, const std::string &PixelShaderFilePath);
    ~DXPipeline();
    DXPipeline &operator=(const DXPipeline &) = delete;
    DXPipeline(const DXPipeline &) = delete;

public:
    inline ID3D12PipelineState *GetPipelineStateObject() const { return PipelineStateObject.Get(); }

private:
    bool Init();
    void CreateGraphicsPipeline(std::vector<char> &VertexShaderCode, std::vector<char> &PixelShaderCode);
    std::vector<char> ReadFile(const std::string &FilePath);
    // Initializers
    ID3D12RootSignature *CreateRootSignature();
    D3D12_RASTERIZER_DESC InitRasterizerDesc();
    D3D12_STREAM_OUTPUT_DESC InitStreamOutputDesc();
    D3D12_BLEND_DESC InitBlendDesc();
    D3D12_DEPTH_STENCIL_DESC InitDepthStencilDesc();

private:
    ID3D12Device14 &DeviceRef;
    ComPtr<ID3D12PipelineState> PipelineStateObject;
    ComPtr<ID3D12RootSignature> RootSignature;
};