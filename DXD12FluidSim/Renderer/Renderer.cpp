#include "Renderer/Renderer.hpp"
#include "D3D/DXSwapchain.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "D3D/DXGraphicsPipeline.hpp"
#include <iostream>
#include "GlobInclude/Utils.hpp"
#include "D3D/DXComputePipeline.hpp"

Renderer::Renderer(DXSwapchain &Swapchain, ID3D12Device14 &Device) : SwapchainRef(Swapchain), DeviceRef(Device)
{
    GraphicsPipeline = std::make_unique<DXGraphicsPipeline>(DeviceRef);
    GraphicsPipeline->SetRootSignature(CreateGraphicsRootSig());
    GraphicsPipeline->CreatePipeline(SHADER_PATH "Triangle_vs.cso", SHADER_PATH "Triangle_ps.cso");
    ComputePipeline = std::make_unique<DXComputePipeline>(DeviceRef);
    ComputePipeline->SetRootSignature(CreateComputeRootSig());
    ComputePipeline->CreatePipeline(SHADER_PATH "Test_cs.cso", 512, 512, 1);
}

Renderer::~Renderer() {}

void Renderer::BeginFrame(ID3D12GraphicsCommandList7 *CmdList)
{
    UpdateCameraBuffer();
    ID3D12Resource1 *CurrentBuffer = SwapchainRef.GetCurrentBackBuffer();
    VALIDATE_PTR(CurrentBuffer);

    Utils::TransitionResoure(CmdList, CurrentBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

    float ClearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    CmdList->ClearRenderTargetView(SwapchainRef.GetCurrentRTVHandle(), ClearColor, 0, nullptr);
    CmdList->OMSetRenderTargets(1, &SwapchainRef.GetCurrentRTVHandle(), FALSE, &SwapchainRef.GetCurrentDSVHandle());
    CmdList->ClearDepthStencilView(SwapchainRef.GetCurrentDSVHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    Utils::TransitionResoure(
        CmdList,
        ComputePipeline->GetOutputTexture(),
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS
    );

    ComputePipeline->Dispatch(CmdList);

    Utils::TransitionResoure(
        CmdList,
        ComputePipeline->GetOutputTexture(),
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );

    GraphicsPipeline->Dispatch(CmdList);
    CmdList->SetGraphicsRootConstantBufferView(0, CameraBufferGPUAddress);
    CmdList->SetGraphicsRootDescriptorTable(2, ComputePipeline->GetSRVGPUHandle());

    CmdList->RSSetViewports(1, &Viewport);
    CmdList->RSSetScissorRects(1, &SwapchainRef.GetScissorRect());

    RenderGameObject(CmdList);
}

void Renderer::EndFrame(ID3D12GraphicsCommandList7 *CmdList)
{
    ID3D12Resource1 *CurrentBuffer = SwapchainRef.GetCurrentBackBuffer();
    VALIDATE_PTR(CurrentBuffer);

    Utils::TransitionResoure(CmdList, CurrentBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}


void Renderer::InitializeBuffers(ID3D12GraphicsCommandList7 *CmdList)
{
    Camera.SetPosition({0.0f, 0.0f, -5.0f});
    Camera.SetTarget({0.0f, 0.0f, 0.0f});
    Camera.SetLens(DirectX::XM_PIDIV4, SwapchainRef.GetAspectRatio(), 0.1f, 1000.0f);

    UINT CbSize = (sizeof(CameraBufferConstants) + 255) & ~255;
    CameraBufferConstants CbData;
    CbData.ViewProjection = DirectX::XMMatrixTranspose(Camera.GetViewProjection());

    Utils::CreateUploadBuffer(DeviceRef, CmdList, CbSize, &CbData, CameraBuffer_Default, CameraBuffer_Upload);
    CameraBufferGPUAddress = CameraBuffer_Default->GetGPUVirtualAddress();
}

void Renderer::UpdateCameraBuffer()
{
    CameraBufferConstants CBData;
    CBData.ViewProjection = DirectX::XMMatrixTranspose(Camera.GetViewProjection());
    void *mapped = nullptr;
    CameraBuffer_Upload->Map(0, nullptr, &mapped);
    memcpy(mapped, &CBData, sizeof(CBData));
    CameraBuffer_Upload->Unmap(0, nullptr);
}

void Renderer::RegisterGameObject(GameObject *GameObj, ID3D12GraphicsCommandList7 *CmdList)
{
    GameObj->Transform.UpdateMatrix();
    TransformConstants CBData{GameObj->Transform.ModelMatrix};

    UINT CBSize = (sizeof(TransformConstants) + 255) & ~255;

    GameObjectGPUData Data;
    Utils::CreateDynamicUploadBuffer(DeviceRef, CBSize, Data.TransformBuffer_Upload, Data.MappedPtr);

    memcpy(Data.MappedPtr, &CBData, sizeof(CBData));
    Data.GPUAddress = Data.TransformBuffer_Upload->GetGPUVirtualAddress();

    GameObjectResources[GameObj] = std::move(Data);
    RegisteredObjects.push_back(GameObj);
}



void Renderer::RenderGameObject(ID3D12GraphicsCommandList7 *CmdList)
{
    for (auto *OBJ : RegisteredObjects)
    {
        OBJ->Transform.UpdateMatrix();
        RotX += 0.01f;
        RotY += 0.01f;

        OBJ->Transform.Rotation = {RotX, RotY, 0.0f};
        OBJ->Transform.UpdateMatrix();

        auto &Data = GameObjectResources[OBJ];
        TransformConstants CBData{OBJ->Transform.ModelMatrix};

        memcpy(Data.MappedPtr, &CBData, sizeof(CBData));

        CmdList->SetGraphicsRootConstantBufferView(1, Data.GPUAddress);
        OBJ->Mesh->Bind(CmdList);
        CmdList->DrawIndexedInstanced(OBJ->Mesh->GetIndexCount(), 1, 0, 0, 0);
    }
}

ComPtr<ID3D12RootSignature> Renderer::CreateComputeRootSig()
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
        DeviceRef.CreateRootSignature(
            0, Serialized->GetBufferPointer(), Serialized->GetBufferSize(), IID_PPV_ARGS(&RootSig)
        ),
        RootSig
    );

    return RootSig;
}

ComPtr<ID3D12RootSignature> Renderer::CreateGraphicsRootSig()
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
        DeviceRef.CreateRootSignature(
            0, RootSigBlob->GetBufferPointer(), RootSigBlob->GetBufferSize(), IID_PPV_ARGS(&RootSignature)
        ),
        RootSignature
    );

    return RootSignature;
}

void Renderer::OnResize(float NewAspectRatio) { Camera.SetLens(DirectX::XM_PIDIV4, NewAspectRatio, 0.1f, 1000.0f); }