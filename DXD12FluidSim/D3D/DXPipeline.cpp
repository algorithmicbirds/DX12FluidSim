#include "D3D/DXPipeline.hpp"
#include "Vertex.hpp"
#include <fstream>
#include "DebugLayer/DebugMacros.hpp"

DXPipeline::DXPipeline(
    ID3D12Device14 &Device, const std::string &VertexShaderFilePath, const std::string &PixelShaderFilePath
)
    : DeviceRef(Device)
{
    std::vector<char> VertexShaderFile = ReadFile(VertexShaderFilePath);
    std::vector<char> PixelShaderFile = ReadFile(PixelShaderFilePath);
    CreateGraphicsPipeline(VertexShaderFile, PixelShaderFile);
}

DXPipeline::~DXPipeline() {}

void DXPipeline::CreateGraphicsPipeline(std::vector<char> &VertexShaderCode, std::vector<char> &PixelShaderCode)
{
    ComPtr<ID3DBlob> RootSigBlob;
    ComPtr<ID3DBlob> RootErrBlob;

    D3D12_ROOT_SIGNATURE_DESC RootSigDesc{};
    RootSigDesc.NumParameters = 0;
    RootSigDesc.pParameters = nullptr;
    RootSigDesc.NumStaticSamplers = 0;
    RootSigDesc.pStaticSamplers = nullptr;
    RootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    DX_VALIDATE(
        D3D12SerializeRootSignature(&RootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &RootSigBlob, &RootErrBlob), RootSigBlob
    );

    ComPtr<ID3D12RootSignature> RootSig;

    DX_VALIDATE(
        DeviceRef.CreateRootSignature(
            0, RootSigBlob->GetBufferPointer(), RootSigBlob->GetBufferSize(), IID_PPV_ARGS(&RootSig)
        ),
        RootSig
    );

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

    GraphicsPSODesc.pRootSignature = RootSig.Get();
    GraphicsPSODesc.RasterizerState = InitRasterizerDesc();
    GraphicsPSODesc.StreamOutput = InitStreamOutputDesc();
    
}

std::vector<char> DXPipeline::ReadFile(const std::string &FilePath)
{
    std::ifstream File{FilePath, std::ios::ate | std::ios::binary};
    if (!File.is_open())
    {
        throw std::runtime_error("File failed to open");
    }

    size_t FileSize = static_cast<size_t>(File.tellg());
    std::vector<char> Buffer(FileSize);
    File.seekg(0);
    File.read(Buffer.data(), FileSize);
    File.close();

    return Buffer;
}

D3D12_RASTERIZER_DESC DXPipeline::InitRasterizerDesc()
{
    D3D12_RASTERIZER_DESC RasterizerDesc{};
    RasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    RasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    RasterizerDesc.FrontCounterClockwise = FALSE;
    RasterizerDesc.DepthBias = 0;
    RasterizerDesc.DepthBiasClamp = 0.0f;
    RasterizerDesc.SlopeScaledDepthBias = 0.0f;
    RasterizerDesc.DepthClipEnable = FALSE;
    RasterizerDesc.MultisampleEnable = FALSE;
    RasterizerDesc.AntialiasedLineEnable = FALSE;
    RasterizerDesc.ForcedSampleCount = 0;
    return RasterizerDesc;
}

D3D12_STREAM_OUTPUT_DESC DXPipeline::InitStreamOutputDesc() { 
    D3D12_STREAM_OUTPUT_DESC StreamOutPutDesc{};
    StreamOutPutDesc.NumEntries = 0;
    StreamOutPutDesc.NumStrides = 0;
    StreamOutPutDesc.pBufferStrides = nullptr;
    StreamOutPutDesc.pSODeclaration = nullptr;
    StreamOutPutDesc.RasterizedStream = 0;
    return StreamOutPutDesc; 
}
