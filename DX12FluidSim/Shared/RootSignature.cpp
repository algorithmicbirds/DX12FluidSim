#include "Shared/RootSignature.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "Shared/RootParams.hpp"
#include <iostream>

namespace RootSignature
{
ComPtr<ID3D12RootSignature> CreateComputeRootSig(ID3D12Device14 &Device)
{
    D3D12_DESCRIPTOR_RANGE1 ParticleRangeUAV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0);
    D3D12_DESCRIPTOR_RANGE1 ParticleHashRangeUAV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1);
    D3D12_DESCRIPTOR_RANGE1 ParticleSortRangeUAV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 2);
    D3D12_DESCRIPTOR_RANGE1 CellStartRangeUAV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 3);
    D3D12_DESCRIPTOR_RANGE1 CellEndRangeUAV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 4);
    D3D12_DESCRIPTOR_RANGE1 RadixHistogramUAV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 5);
    D3D12_DESCRIPTOR_RANGE1 RadixPrefixSumUAV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 6);
    D3D12_DESCRIPTOR_RANGE1 RadixScatterUAV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 7);
    D3D12_DESCRIPTOR_RANGE1 RadixScatterBinOffsetUAV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 8);

    D3D12_DESCRIPTOR_RANGE1 ParticleIntegrateRangeSRV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0);
    D3D12_DESCRIPTOR_RANGE1 ParticlePrevPositionsRangeSRV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1);
    D3D12_DESCRIPTOR_RANGE1 ParticleHashRangeSRV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2);
    D3D12_DESCRIPTOR_RANGE1 SortedHashSRV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3);
    D3D12_DESCRIPTOR_RANGE1 CellStartRangeSRV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4);
    D3D12_DESCRIPTOR_RANGE1 CellEndRangeSRV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5);
    D3D12_DESCRIPTOR_RANGE1 RadixHistogramSRV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 6);
    D3D12_DESCRIPTOR_RANGE1 RadixPrefixSumSRV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 7);
    D3D12_DESCRIPTOR_RANGE1 RadixScatterSRV = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 8);


    D3D12_ROOT_PARAMETER1 RootParam[23] = {};

    RootParam[ComputeRootParams::TimerCB_b0] = CreateCBV(0);
    RootParam[ComputeRootParams::BoundingBoxCB_b1] = CreateCBV(1);
    RootParam[ComputeRootParams::ComputeSimParamsCB_b2] = CreateCBV(2);
    RootParam[ComputeRootParams::PrecomputedKernalCB_b3] = CreateCBV(3);    
    RootParam[ComputeRootParams::RadixHistogram_b4] = CreateCBV(4);    

    RootParam[ComputeRootParams::ParticleForcesUAV_u0] = CreateTableParam(ParticleRangeUAV);
    RootParam[ComputeRootParams::ParticleHashUAV_u1] = CreateTableParam(ParticleHashRangeUAV);
    RootParam[ComputeRootParams::ParticleSortUAV_u2] = CreateTableParam(ParticleSortRangeUAV);
    RootParam[ComputeRootParams::CellStartUAV_u3] = CreateTableParam(CellStartRangeUAV);
    RootParam[ComputeRootParams::CellEndUAV_u4] = CreateTableParam(CellEndRangeUAV);
    RootParam[ComputeRootParams::RadixHistogramUAV_u5] = CreateTableParam(RadixHistogramUAV);
    RootParam[ComputeRootParams::RadixPrefixSumUAV_u6] = CreateTableParam(RadixPrefixSumUAV);
    RootParam[ComputeRootParams::RadixScatterUAV_u7] = CreateTableParam(RadixScatterUAV);
    RootParam[ComputeRootParams::RadixScatterBinOffsetsUAV_u8] = CreateTableParam(RadixScatterBinOffsetUAV);

    RootParam[ComputeRootParams::ParticleForcesSRV_t0] = CreateTableParam(ParticleIntegrateRangeSRV);
    RootParam[ComputeRootParams::ParticlePrevPositionsSRV_t1] = CreateTableParam(ParticlePrevPositionsRangeSRV);
    RootParam[ComputeRootParams::ParticleHashSRV_t2] = CreateTableParam(ParticleHashRangeSRV);
    RootParam[ComputeRootParams::SortedHashSRV_t3] = CreateTableParam(SortedHashSRV);
    RootParam[ComputeRootParams::CellStartSRV_t4] = CreateTableParam(CellStartRangeSRV);
    RootParam[ComputeRootParams::CellEndSRV_t5] = CreateTableParam(CellEndRangeSRV);
    RootParam[ComputeRootParams::RadixHistogramSRV_t6] = CreateTableParam(RadixHistogramSRV);
    RootParam[ComputeRootParams::RadixPrefixSumSRV_t7] = CreateTableParam(RadixPrefixSumSRV);
    RootParam[ComputeRootParams::RadixScatterSRV_t8] = CreateTableParam(RadixScatterSRV);

    ComPtr<ID3D12RootSignature> RootSig;
    RootSig = CreateRootSig(Device, RootParam, _countof(RootParam));
    return RootSig;
}

ComPtr<ID3D12RootSignature> CreateGraphicsRootSig(ID3D12Device14 &Device)
{
    D3D12_DESCRIPTOR_RANGE1 ParticlesSRVRange = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0);
    D3D12_DESCRIPTOR_RANGE1 DensitySRVRange = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1);
    D3D12_DESCRIPTOR_RANGE1 DebugRange = CreateRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0);

    D3D12_ROOT_PARAMETER1 RootParam[7] = {};

    RootParam[GraphicsRootParams::CameraCB_b0] = CreateCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
    RootParam[GraphicsRootParams::ModelCB_b1] = CreateCBV(1, D3D12_SHADER_VISIBILITY_VERTEX);
    RootParam[GraphicsRootParams::TimerCB_b2] = CreateCBV(2, D3D12_SHADER_VISIBILITY_VERTEX);
    RootParam[GraphicsRootParams::BoundingBoxCB_b3] = CreateCBV(3, D3D12_SHADER_VISIBILITY_VERTEX);
    RootParam[GraphicsRootParams::GraphicsSimParams_b4] = CreateCBV(4, D3D12_SHADER_VISIBILITY_PIXEL);
    RootParam[GraphicsRootParams::ParticleForcesSRV_t0] = CreateTableParam(ParticlesSRVRange);
    RootParam[GraphicsRootParams::DebugUAV_u0] = CreateTableParam(DebugRange);

    ComPtr<ID3D12RootSignature> RootSignature;
    RootSignature = CreateRootSig(
        Device, RootParam, _countof(RootParam), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
    );
    return RootSignature;
}
} // namespace RootSignature