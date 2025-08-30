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

private:
    bool Init();
    void CreateGraphicsPipeline(std::vector<char> &VertexShaderCode, std::vector<char> &PixelShaderCode);
    std::vector<char> ReadFile(const std::string &FilePath);

private:
    ID3D12Device14 &DeviceRef;

    //Initializers

    D3D12_RASTERIZER_DESC InitRasterizerDesc();
    D3D12_STREAM_OUTPUT_DESC InitStreamOutputDesc();
};