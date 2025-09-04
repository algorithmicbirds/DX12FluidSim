#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>
#include <string>

class DXGraphicsPipeline
{
public:
    DXGraphicsPipeline(ID3D12Device14 &Device, const std::string &VertexShaderFilePath, const std::string &PixelShaderFilePath);
    ~DXGraphicsPipeline();
    DXGraphicsPipeline &operator=(const DXGraphicsPipeline &) = delete;
    DXGraphicsPipeline(const DXGraphicsPipeline &) = delete;

public:
    inline ID3D12PipelineState *GetPipelineStateObject() const { return PipelineStateObject.Get(); }
    inline ID3D12RootSignature *GetRootSignature() const { return RootSignature.Get(); }

private:
    bool Init();
    void CreateGraphicsPipeline(std::vector<char> &VertexShaderCode, std::vector<char> &PixelShaderCode);
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