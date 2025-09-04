#include "D3D/DXGraphicsPipeline.hpp"
#include "Vertex.hpp"
#include <fstream>
#include "DebugLayer/DebugMacros.hpp"
#include "GlobInclude/Utils.hpp"

DXGraphicsPipeline::DXGraphicsPipeline(ID3D12Device14 &Device) : DeviceRef(Device) {}

DXGraphicsPipeline::~DXGraphicsPipeline() {}

void DXGraphicsPipeline::CreatePipeline(const std::string &VertexShaderFilePath, const std::string &PixelShaderFilePath) {
    std::vector<char> VertexShaderFile = Utils::ReadFile(VertexShaderFilePath);
    std::vector<char> PixelShaderFile = Utils::ReadFile(PixelShaderFilePath);

    CreatePipelineState(VertexShaderFile, PixelShaderFile);
}

void DXGraphicsPipeline::CreatePipelineState(std::vector<char> &VertexShaderCode, std::vector<char> &PixelShaderCode)
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC GraphicsPSODesc{};

    GraphicsPSODesc.InputLayout = Vertex::GetInputLayout();
    GraphicsPSODesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

    GraphicsPSODesc.VS.BytecodeLength = VertexShaderCode.size();
    GraphicsPSODesc.VS.pShaderBytecode = VertexShaderCode.data();
    GraphicsPSODesc.PS.BytecodeLength = PixelShaderCode.size();
    GraphicsPSODesc.PS.pShaderBytecode = PixelShaderCode.data();
    GraphicsPSODesc.DS.BytecodeLength = 0;
    GraphicsPSODesc.DS.pShaderBytecode = 0;
    GraphicsPSODesc.GS.BytecodeLength = 0;
    GraphicsPSODesc.GS.pShaderBytecode = 0;
    GraphicsPSODesc.HS.BytecodeLength = 0;
    GraphicsPSODesc.HS.pShaderBytecode = 0;

    GraphicsPSODesc.pRootSignature = RootSignature.Get();
    GraphicsPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    GraphicsPSODesc.RasterizerState = InitRasterizerDesc();
    GraphicsPSODesc.StreamOutput = InitStreamOutputDesc();
    GraphicsPSODesc.BlendState = InitBlendDesc();
    GraphicsPSODesc.DepthStencilState = InitDepthStencilDesc();

    GraphicsPSODesc.SampleMask = 0xFFFFFFFF;
    GraphicsPSODesc.SampleDesc.Count = 1;
    GraphicsPSODesc.SampleDesc.Quality = 0;

    GraphicsPSODesc.NumRenderTargets = 1;
    GraphicsPSODesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    GraphicsPSODesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

    GraphicsPSODesc.CachedPSO.CachedBlobSizeInBytes = 0;
    GraphicsPSODesc.CachedPSO.pCachedBlob = nullptr;
    GraphicsPSODesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    DX_VALIDATE(
        DeviceRef.CreateGraphicsPipelineState(&GraphicsPSODesc, IID_PPV_ARGS(&PipelineStateObject)), PipelineStateObject
    );
}

D3D12_RASTERIZER_DESC DXGraphicsPipeline::InitRasterizerDesc()
{
    D3D12_RASTERIZER_DESC RasterizerDesc{};
    RasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    RasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    RasterizerDesc.FrontCounterClockwise = FALSE;
    RasterizerDesc.DepthBias = 0;
    RasterizerDesc.DepthBiasClamp = 0.0f;
    RasterizerDesc.SlopeScaledDepthBias = 0.0f;
    RasterizerDesc.DepthClipEnable = TRUE;
    RasterizerDesc.MultisampleEnable = FALSE;
    RasterizerDesc.AntialiasedLineEnable = FALSE;
    RasterizerDesc.ForcedSampleCount = 0;
    return RasterizerDesc;
}

D3D12_STREAM_OUTPUT_DESC DXGraphicsPipeline::InitStreamOutputDesc()
{
    D3D12_STREAM_OUTPUT_DESC StreamOutPutDesc{};
    StreamOutPutDesc.NumEntries = 0;
    StreamOutPutDesc.NumStrides = 0;
    StreamOutPutDesc.pBufferStrides = nullptr;
    StreamOutPutDesc.pSODeclaration = nullptr;
    StreamOutPutDesc.RasterizedStream = 0;
    return StreamOutPutDesc;
}

D3D12_BLEND_DESC DXGraphicsPipeline::InitBlendDesc()
{
    D3D12_BLEND_DESC BlendDesc{};
    BlendDesc.AlphaToCoverageEnable = FALSE;
    BlendDesc.IndependentBlendEnable = FALSE;
    BlendDesc.RenderTarget[0].BlendEnable = FALSE;
    BlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
    BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
    BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
    BlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
    BlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    BlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    return BlendDesc;
}

D3D12_DEPTH_STENCIL_DESC DXGraphicsPipeline::InitDepthStencilDesc()
{
    D3D12_DEPTH_STENCIL_DESC DepthStencilDesc{};
    DepthStencilDesc.DepthEnable = TRUE;
    DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    DepthStencilDesc.StencilEnable = FALSE;
    DepthStencilDesc.StencilReadMask = 0;
    DepthStencilDesc.StencilWriteMask = 0;
    DepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    DepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    DepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    DepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    DepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    DepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    DepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    return DepthStencilDesc;
}

void DXGraphicsPipeline::Dispatch(ID3D12GraphicsCommandList7 *CmdList) {
    CmdList->SetPipelineState(PipelineStateObject.Get());
    CmdList->SetGraphicsRootSignature(RootSignature.Get());
};