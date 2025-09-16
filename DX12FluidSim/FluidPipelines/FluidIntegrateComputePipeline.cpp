#include "FluidIntegrateComputePipeline.hpp"
#include "Shared/Utils.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "FluidPipelines/FluidPipelinesHeapDesc.hpp"


FluidIntegrateComputePipeline::FluidIntegrateComputePipeline(ID3D12Device14 &Device) :DeviceRef(Device) {}

FluidIntegrateComputePipeline::~FluidIntegrateComputePipeline() {}

void FluidIntegrateComputePipeline::CreatePipeline(const std::string& CSFilePath, FluidHeapDescriptor& HeapDesc)
{
    std::vector<char> CSFile = Utils::ReadFile(CSFilePath);
    CreateBufferDesc(HeapDesc);
    CreatePipelineState(CSFile);
}

void FluidIntegrateComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7* CmdList)
{
    UINT StructuredBufferSize = sizeof(ParticleIntegrateSB) * ParticleCount;
    std::vector<ParticleIntegrateSB> Particles(ParticleCount);

    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        StructuredBufferSize,
        Particles.data(),
        ParticleGPU.DefaultBuffer,
        ParticleGPU.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );
}

void FluidIntegrateComputePipeline::CreateBufferDesc(FluidHeapDescriptor& HeapDesc)
{
    ParticleIntegrateUAVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::UAV, ParticleGPU.DefaultBuffer, ParticleCount, sizeof(ParticleIntegrateSB)
    ); 

     ParticleIntegrateSRVGPUHandle = HeapDesc.AllocateDescriptor(
        DescriptorType::SRV, ParticleGPU.DefaultBuffer, ParticleCount, sizeof(ParticleIntegrateSB)
    );
}

void FluidIntegrateComputePipeline::CreatePipelineState(const std::vector<char> &CSCode)
{
    D3D12_COMPUTE_PIPELINE_STATE_DESC Desc{};
    Desc.pRootSignature = RootSignature.Get();
    Desc.CS.BytecodeLength = CSCode.size();
    Desc.CS.pShaderBytecode = CSCode.data();

    DX_VALIDATE(DeviceRef.CreateComputePipelineState(&Desc, IID_PPV_ARGS(&PipelineStateObject)), PipelineStateObject);
}


void FluidIntegrateComputePipeline::BindRootAndPSO(ID3D12GraphicsCommandList7 *CmdList)
{
    CmdList->SetPipelineState(PipelineStateObject.Get());
    CmdList->SetGraphicsRootSignature(RootSignature.Get());
};