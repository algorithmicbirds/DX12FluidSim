#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "DebugLayer/DebugMacros.hpp"

namespace RootSignature
{
ComPtr<ID3D12RootSignature> CreateGraphicsRootSig(ID3D12Device14 &Device);
ComPtr<ID3D12RootSignature> CreateComputeRootSig(ID3D12Device14 &Device);
inline D3D12_ROOT_PARAMETER1 CreateCBV(
    UINT ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL, UINT RegisterSpace = 0
)
{
    D3D12_ROOT_PARAMETER1 Param{};
    Param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    Param.Descriptor.ShaderRegister = ShaderRegister;
    Param.Descriptor.RegisterSpace = RegisterSpace;
    Param.ShaderVisibility = ShaderVisibility;
    return Param;
}
inline D3D12_DESCRIPTOR_RANGE1 CreateRange(
    D3D12_DESCRIPTOR_RANGE_TYPE RangeType,
    UINT BaseRegister,
    UINT NumOfDescriptors = 1,
    UINT RegisterSpace = 0,
    D3D12_DESCRIPTOR_RANGE_FLAGS Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE
)
{
    D3D12_DESCRIPTOR_RANGE1 Range{};
    Range.RangeType = RangeType;
    Range.NumDescriptors = NumOfDescriptors;
    Range.BaseShaderRegister = BaseRegister;
    Range.RegisterSpace = RegisterSpace;
    Range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    Range.Flags = Flags;
    return Range;
}
inline D3D12_ROOT_PARAMETER1 CreateTableParam(
    D3D12_DESCRIPTOR_RANGE1 &Range,
    D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL,
    UINT NumDescriptorRanges = 1u
)
{
    D3D12_ROOT_DESCRIPTOR_TABLE1 Table = {};
    Table.NumDescriptorRanges = NumDescriptorRanges;
    Table.pDescriptorRanges = &Range;

    D3D12_ROOT_PARAMETER1 Param{};
    Param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    Param.ShaderVisibility = ShaderVisibility;
    Param.DescriptorTable = Table;
    return Param;
}
inline ComPtr<ID3D12RootSignature> CreateRootSig(
    ID3D12Device14 &Device,
    D3D12_ROOT_PARAMETER1 *RootParams,
    UINT NumParams,
    D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE
)
{
    ComPtr<ID3DBlob> RootSigBlob;
    ComPtr<ID3DBlob> RootErrBlob;

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootSigDesc{};
    RootSigDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;

    RootSigDesc.Desc_1_1.NumParameters = NumParams;
    RootSigDesc.Desc_1_1.pParameters = RootParams;
    RootSigDesc.Desc_1_1.NumStaticSamplers = 0;
    RootSigDesc.Desc_1_1.pStaticSamplers = nullptr;
    RootSigDesc.Desc_1_1.Flags = Flags;

    DX_VALIDATE(D3D12SerializeVersionedRootSignature(&RootSigDesc, &RootSigBlob, &RootErrBlob), RootSigBlob);

    ComPtr<ID3D12RootSignature> RootSignature;
    DX_VALIDATE(
        Device.CreateRootSignature(
            0, RootSigBlob->GetBufferPointer(), RootSigBlob->GetBufferSize(), IID_PPV_ARGS(&RootSignature)
        ),
        RootSignature
    );

    return RootSignature;
}
} // namespace RootSignatures