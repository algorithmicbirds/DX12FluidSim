#include "Renderer/Renderer.hpp"
#include "D3D/DXSwapchain.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "D3D/DXGraphicsPipeline.hpp"
#include <iostream>
#include "GlobInclude/Utils.hpp"
#include "D3D/DXComputePipeline.hpp"

Renderer::Renderer(DXSwapchain &Swapchain, ID3D12Device14 &Device) : SwapchainRef(Swapchain), DeviceRef(Device)
{
    Pipeline = std::make_unique<DXGraphicsPipeline>(DeviceRef, SHADER_PATH "Triangle_vs.cso", SHADER_PATH "Triangle_ps.cso");
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

    CmdList->SetPipelineState(Pipeline->GetPipelineStateObject());
    CmdList->SetGraphicsRootSignature(Pipeline->GetRootSignature());
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


void Renderer::OnResize(float NewAspectRatio) { Camera.SetLens(DirectX::XM_PIDIV4, NewAspectRatio, 0.1f, 1000.0f); }