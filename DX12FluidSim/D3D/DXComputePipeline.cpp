#include "DXComputePipeline.hpp"
#include "Shared/Utils.hpp"
#include "DebugLayer/DebugMacros.hpp"

DXComputePipeline::DXComputePipeline(ID3D12Device14 &Device) : DeviceRef(Device) {}

DXComputePipeline::~DXComputePipeline() {}

void DXComputePipeline::CreatePipeline(const std::string &CSFilePath)
{
    std::vector<char> CSCode = Utils::ReadFile(CSFilePath);

    CreateDescHeap();
    CreateUAVDesc();
    CreateSRVDesc();
    CreatePipelineState(CSCode);
}

void DXComputePipeline::CreateDescHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC HeapDesc{};
    HeapDesc.NumDescriptors = 2;
    HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    DX_VALIDATE(DeviceRef.CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&DescriptorHeap)), DescriptorHeap);

    auto gpuStart = DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    auto handleSize = DeviceRef.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    UAVGPUHandle = gpuStart;
    SRVGPUHandle.ptr = gpuStart.ptr + handleSize;
}

void DXComputePipeline::CreateUAVDesc()
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};
    UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
    UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    UAVDesc.Buffer.NumElements = ParticleCount;
    UAVDesc.Buffer.StructureByteStride = sizeof(Particle);
    UAVDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

    DeviceRef.CreateUnorderedAccessView(
        ParticleData.DefaultBuffer.Get(), nullptr, &UAVDesc, DescriptorHeap->GetCPUDescriptorHandleForHeapStart()
    );
}

void DXComputePipeline::CreateSRVDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
    SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
    SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    SRVDesc.Buffer.NumElements = ParticleCount;
    SRVDesc.Buffer.StructureByteStride = sizeof(Particle);
    SRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

    UINT HandleSize = DeviceRef.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3D12_CPU_DESCRIPTOR_HANDLE SRVHandle = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    SRVHandle.ptr += HandleSize;

    DeviceRef.CreateShaderResourceView(ParticleData.DefaultBuffer.Get(), &SRVDesc, SRVHandle);
}

void DXComputePipeline::CreateStructuredBuffer(ID3D12GraphicsCommandList7 *CmdList, UINT Count)
{
    ParticleCount = Count;
    UINT BufferSize = sizeof(Particle) * ParticleCount;

    std::vector<Particle> InitData(BufferSize / sizeof(Particle));
    for (auto &p : InitData)
        p.Position = {0.0f, 0.0f, 0.0f};

    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        BufferSize,
        InitData.data(),
        ParticleData.DefaultBuffer,
        ParticleData.UploadBuffer,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );
}

void DXComputePipeline::CreatePipelineState(const std::vector<char> &CSCode)
{
    VALIDATE_PTR(RootSignature.Get());

    D3D12_COMPUTE_PIPELINE_STATE_DESC Desc{};
    Desc.pRootSignature = RootSignature.Get();
    Desc.CS.BytecodeLength = CSCode.size();
    Desc.CS.pShaderBytecode = CSCode.data();

    DX_VALIDATE(DeviceRef.CreateComputePipelineState(&Desc, IID_PPV_ARGS(&PipelineState)), PipelineState);
}

void DXComputePipeline::Dispatch(ID3D12GraphicsCommandList7 *CmdList)
{
    CmdList->SetPipelineState(PipelineState.Get());
    CmdList->SetComputeRootSignature(RootSignature.Get());
}
