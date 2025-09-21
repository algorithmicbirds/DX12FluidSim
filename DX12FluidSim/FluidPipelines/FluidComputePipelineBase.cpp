#include "FluidPipelines/FluidComputePipelineBase.hpp"

FluidComputePipelineBase::FluidComputePipelineBase(ID3D12Device14 &Device) : DeviceRef(Device) {}

FluidComputePipelineBase::~FluidComputePipelineBase() {}

void FluidComputePipelineBase::SetRootSignature(ComPtr<ID3D12RootSignature> InRootSig) { RootSignature = InRootSig; }

void FluidComputePipelineBase::CreatePipeline(const std::string &CSFilePath, FluidHeapDescriptor &HeapDesc)
{
    CreateBufferDesc(HeapDesc); 
    std::vector<char> CSCode = Utils::ReadFile(CSFilePath);
    CreatePipelineState(CSCode); 
}

void FluidComputePipelineBase::BindRootAndPSO(ID3D12GraphicsCommandList7 *CmdList)
{
    CmdList->SetPipelineState(PipelineStateObject.Get());
    CmdList->SetComputeRootSignature(RootSignature.Get());
}

void FluidComputePipelineBase::CreatePipelineState(const std::vector<char> &CSCode)
{
    D3D12_COMPUTE_PIPELINE_STATE_DESC Desc{};
    Desc.pRootSignature = RootSignature.Get();
    Desc.CS.BytecodeLength = CSCode.size();
    Desc.CS.pShaderBytecode = CSCode.data();

    DX_VALIDATE(DeviceRef.CreateComputePipelineState(&Desc, IID_PPV_ARGS(&PipelineStateObject)), PipelineStateObject);
}
