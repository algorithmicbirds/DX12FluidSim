#include "DXComputePipeline.hpp"
#include "GlobInclude/Utils.hpp"
#include "DebugLayer/DebugMacros.hpp"

DXComputePipeline::DXComputePipeline(ID3D12Device14 &Device) : DeviceRef(Device) {}

DXComputePipeline::~DXComputePipeline() {}

void DXComputePipeline::CreatePipeline(const std::string &CSFilePath, UINT Width, UINT Height, UINT UAVIndexIn)
{
    TexWidth = Width;
    TexHeight = Height;
    UAVIndex = UAVIndexIn;

    std::vector<char> CSCode = Utils::ReadFile(CSFilePath);

    CreateTexture();
    CreateDescHeap();
    CreateUAVDesc();
    CreateSRVDesc();
    CreatePipelineState(CSCode);
}

void DXComputePipeline::CreateTexture()
{
    D3D12_RESOURCE_DESC texDesc{};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = TexWidth;
    texDesc.Height = TexHeight;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    texDesc.SampleDesc.Count = 1;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    DX_VALIDATE(
        DeviceRef.CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &texDesc,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            nullptr,
            IID_PPV_ARGS(&OutputTexture)
        ),
        OutputTexture
    );
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
    UAVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

    DeviceRef.CreateUnorderedAccessView(
        OutputTexture.Get(), nullptr, &UAVDesc, DescriptorHeap->GetCPUDescriptorHandleForHeapStart()
    );
}

void DXComputePipeline::CreateSRVDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
    SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MipLevels = 1;
    SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    UINT handleSize = DeviceRef.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    srvHandle.ptr += handleSize;

    DeviceRef.CreateShaderResourceView(OutputTexture.Get(), &SRVDesc, srvHandle);
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

    ID3D12DescriptorHeap *heaps[] = {DescriptorHeap.Get()};
    CmdList->SetDescriptorHeaps(1, heaps);

    CmdList->SetComputeRootDescriptorTable(0, UAVGPUHandle);

    CmdList->Dispatch((TexWidth + 15) / 16, (TexHeight + 15) / 16, 1);
}
