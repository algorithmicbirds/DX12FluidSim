#include "Renderer/RootSignature.hpp"
#include "DebugLayer/DebugMacros.hpp"

namespace RootSignature
{
ComPtr<ID3D12RootSignature> CreateComputeRootSig(ID3D12Device14 &Device)
{
    // Descriptor range for UAV (u0)
    D3D12_DESCRIPTOR_RANGE1 Range = {};
    Range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    Range.NumDescriptors = 1;
    Range.BaseShaderRegister = 0;
    Range.RegisterSpace = 0;
    Range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    Range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

    D3D12_ROOT_PARAMETER1 Param = {};
    Param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    Param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    D3D12_ROOT_DESCRIPTOR_TABLE1 Table = {};
    Table.NumDescriptorRanges = 1;
    Table.pDescriptorRanges = &Range;
    Param.DescriptorTable = Table;

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootDesc = {};
    RootDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
    RootDesc.Desc_1_1.NumParameters = 1;
    RootDesc.Desc_1_1.pParameters = &Param;
    RootDesc.Desc_1_1.NumStaticSamplers = 0;
    RootDesc.Desc_1_1.pStaticSamplers = nullptr;
    RootDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

    ComPtr<ID3DBlob> Serialized;
    ComPtr<ID3DBlob> Error;
    DX_VALIDATE(D3D12SerializeVersionedRootSignature(&RootDesc, &Serialized, &Error), nullptr);

    ComPtr<ID3D12RootSignature> RootSig;
    DX_VALIDATE(
        Device.CreateRootSignature(
            0, Serialized->GetBufferPointer(), Serialized->GetBufferSize(), IID_PPV_ARGS(&RootSig)
        ),
        RootSig
    );

    return RootSig;
}

ComPtr<ID3D12RootSignature> CreateGraphicsRootSig(ID3D12Device14 &Device)
{
    ComPtr<ID3DBlob> RootSigBlob;
    ComPtr<ID3DBlob> RootErrBlob;

    // --- SRV descriptor range (t0) ---
    D3D12_DESCRIPTOR_RANGE SrvRange{};
    SrvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    SrvRange.NumDescriptors = 1;
    SrvRange.BaseShaderRegister = 0;
    SrvRange.RegisterSpace = 0;
    SrvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_PARAMETER RootParam[3] = {};

    // Camera CBV (b0)
    RootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    RootParam[0].Descriptor.ShaderRegister = 0;
    RootParam[0].Descriptor.RegisterSpace = 0;
    RootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    // Model CBV (b1)
    RootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    RootParam[1].Descriptor.ShaderRegister = 1;
    RootParam[1].Descriptor.RegisterSpace = 0;
    RootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    // SRV descriptor table (t0)
    RootParam[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    RootParam[2].DescriptorTable.NumDescriptorRanges = 1;
    RootParam[2].DescriptorTable.pDescriptorRanges = &SrvRange;
    RootParam[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // --- Static sampler (s0) ---
    D3D12_STATIC_SAMPLER_DESC SamplerDesc{};
    SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    SamplerDesc.ShaderRegister = 0;
    SamplerDesc.RegisterSpace = 0;
    SamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC RootSigDesc{};
    RootSigDesc.NumParameters = _countof(RootParam);
    RootSigDesc.pParameters = RootParam;
    RootSigDesc.NumStaticSamplers = 1;
    RootSigDesc.pStaticSamplers = &SamplerDesc;
    RootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    DX_VALIDATE(
        D3D12SerializeRootSignature(&RootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &RootSigBlob, &RootErrBlob), RootSigBlob
    );

    ComPtr<ID3D12RootSignature> RootSignature;

    DX_VALIDATE(
        Device.CreateRootSignature(
            0, RootSigBlob->GetBufferPointer(), RootSigBlob->GetBufferSize(), IID_PPV_ARGS(&RootSignature)
        ),
        RootSignature
    );

    return RootSignature;
}
} // namespace RootSignature