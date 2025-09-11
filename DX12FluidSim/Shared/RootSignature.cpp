#include "Shared/RootSignature.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "Shared/RootParams.hpp"

namespace RootSignature
{
ComPtr<ID3D12RootSignature> CreateComputeRootSig(ID3D12Device14 &Device)
{
    // u0
    D3D12_DESCRIPTOR_RANGE1 Range = {};
    Range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    Range.NumDescriptors = 1;
    Range.BaseShaderRegister = 0;
    Range.RegisterSpace = 0;
    Range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    Range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

   // u1
    D3D12_DESCRIPTOR_RANGE1 DebugRange = {};
    DebugRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    DebugRange.NumDescriptors = 1;
    DebugRange.BaseShaderRegister = 1;
    DebugRange.RegisterSpace = 0;
    DebugRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    DebugRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

    D3D12_ROOT_DESCRIPTOR_TABLE1 Table = {};
    Table.NumDescriptorRanges = 1;
    Table.pDescriptorRanges = &Range;

    
    D3D12_ROOT_DESCRIPTOR_TABLE1 DebugTable = {};
    DebugTable.NumDescriptorRanges = 1;
    DebugTable.pDescriptorRanges = &DebugRange;

    D3D12_ROOT_PARAMETER1 RootParam[6] = {};
    RootParam[ComputeRootParams::ParticleSRV_t0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    RootParam[ComputeRootParams::ParticleSRV_t0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    RootParam[ComputeRootParams::ParticleSRV_t0].DescriptorTable = Table;

    RootParam[ComputeRootParams::TimerCB_b0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    RootParam[ComputeRootParams::TimerCB_b0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    RootParam[ComputeRootParams::TimerCB_b0].Descriptor.ShaderRegister = 0;
    RootParam[ComputeRootParams::TimerCB_b0].Descriptor.RegisterSpace = 0;

    RootParam[ComputeRootParams::BoundingBoxCB_b1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    RootParam[ComputeRootParams::BoundingBoxCB_b1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    RootParam[ComputeRootParams::BoundingBoxCB_b1].Descriptor.ShaderRegister = 1;
    RootParam[ComputeRootParams::BoundingBoxCB_b1].Descriptor.RegisterSpace = 0;

    RootParam[ComputeRootParams::SimParamsCB_b2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    RootParam[ComputeRootParams::SimParamsCB_b2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    RootParam[ComputeRootParams::SimParamsCB_b2].Descriptor.ShaderRegister = 2;
    RootParam[ComputeRootParams::SimParamsCB_b2].Descriptor.RegisterSpace = 0;

    RootParam[ComputeRootParams::PrecomputedKernalCB_b3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    RootParam[ComputeRootParams::PrecomputedKernalCB_b3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    RootParam[ComputeRootParams::PrecomputedKernalCB_b3].Descriptor.ShaderRegister = 3;
    RootParam[ComputeRootParams::PrecomputedKernalCB_b3].Descriptor.RegisterSpace = 0;

    RootParam[ComputeRootParams::DebugSRV_t1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    RootParam[ComputeRootParams::DebugSRV_t1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    RootParam[ComputeRootParams::DebugSRV_t1].DescriptorTable = DebugTable;

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootDesc = {};
    RootDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
    RootDesc.Desc_1_1.NumParameters = _countof(RootParam);
    RootDesc.Desc_1_1.pParameters = RootParam;
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
    D3D12_DESCRIPTOR_RANGE SRVRange{};
    SRVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    SRVRange.NumDescriptors = 1;
    SRVRange.BaseShaderRegister = 0;
    SRVRange.RegisterSpace = 0;
    SRVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_PARAMETER RootParam[5] = {};

    RootParam[GraphicsRootParams::CameraCB_b0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    RootParam[GraphicsRootParams::CameraCB_b0].Descriptor.ShaderRegister = 0;
    RootParam[GraphicsRootParams::CameraCB_b0].Descriptor.RegisterSpace = 0;
    RootParam[GraphicsRootParams::CameraCB_b0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    RootParam[GraphicsRootParams::ModelCB_b1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    RootParam[GraphicsRootParams::ModelCB_b1].Descriptor.ShaderRegister = 1;
    RootParam[GraphicsRootParams::ModelCB_b1].Descriptor.RegisterSpace = 0;
    RootParam[GraphicsRootParams::ModelCB_b1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    RootParam[GraphicsRootParams::ParticleSRV_t0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    RootParam[GraphicsRootParams::ParticleSRV_t0].DescriptorTable.NumDescriptorRanges = 1;
    RootParam[GraphicsRootParams::ParticleSRV_t0].DescriptorTable.pDescriptorRanges = &SRVRange;
    RootParam[GraphicsRootParams::ParticleSRV_t0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    RootParam[GraphicsRootParams::TimerCB_b2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    RootParam[GraphicsRootParams::TimerCB_b2].Descriptor.ShaderRegister = 2;
    RootParam[GraphicsRootParams::TimerCB_b2].Descriptor.RegisterSpace = 0;
    RootParam[GraphicsRootParams::TimerCB_b2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    RootParam[GraphicsRootParams::BoundingBox_b3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    RootParam[GraphicsRootParams::BoundingBox_b3].Descriptor.ShaderRegister = 3;
    RootParam[GraphicsRootParams::BoundingBox_b3].Descriptor.RegisterSpace = 0;
    RootParam[GraphicsRootParams::BoundingBox_b3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    D3D12_ROOT_SIGNATURE_DESC RootSigDesc{};
    RootSigDesc.NumParameters = _countof(RootParam);
    RootSigDesc.pParameters = RootParam;
    RootSigDesc.NumStaticSamplers = 0;
    RootSigDesc.pStaticSamplers = nullptr;
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