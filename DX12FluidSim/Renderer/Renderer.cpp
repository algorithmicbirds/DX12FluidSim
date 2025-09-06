#include "Renderer/Renderer.hpp"
#include "D3D/DXSwapchain.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "D3D/DXGraphicsPipeline.hpp"
#include <iostream>
#include "GlobInclude/Utils.hpp"
#include "D3D/DXComputePipeline.hpp"
#include "Renderer/RootSignature.hpp"

Renderer::Renderer(DXSwapchain &Swapchain, ID3D12Device14 &Device) : SwapchainRef(Swapchain), DeviceRef(Device)
{
    GraphicsPipeline = std::make_unique<DXGraphicsPipeline>(DeviceRef);
    GraphicsPipeline->SetRootSignature(RootSignature::CreateGraphicsRootSig(Device));
    GraphicsPipeline->CreatePipeline(SHADER_PATH "Triangle_vs.cso", SHADER_PATH "Triangle_ps.cso");
    ComputePipeline = std::make_unique<DXComputePipeline>(DeviceRef);
    ComputePipeline->SetRootSignature(RootSignature::CreateComputeRootSig(Device));
    ComputePipeline->CreatePipeline(SHADER_PATH "Test_cs.cso", 512, 512, 1);
}

Renderer::~Renderer() {}

void Renderer::RenderFrame(ID3D12GraphicsCommandList7 *CmdList)
{
    UpdateCameraBuffer();
    ID3D12Resource1 *CurrentBuffer = SwapchainRef.GetCurrentBackBuffer();

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
   // Utils::CreateDynamicUploadBuffer(DeviceRef, CBSize, Data.TransformBuffer_Upload, Data.MappedPtr);
    Utils::CreateUploadBuffer(
        DeviceRef, CmdList, CBSize, &CBData, Data.TransformBuffer_Default, Data.TransformBuffer_Upload
    );

    //memcpy(Data.MappedPtr, &CBData, sizeof(CBData));
    Data.GPUAddress = Data.TransformBuffer_Upload->GetGPUVirtualAddress();

    GameObjectResources[GameObj] = std::move(Data);
    RegisteredObjects.push_back(GameObj);
}



void Renderer::RenderGameObject(ID3D12GraphicsCommandList7 *CmdList)
{
    for (auto *OBJ : RegisteredObjects)
    {
        OBJ->Transform.UpdateMatrix();

        auto &Data = GameObjectResources[OBJ];
        TransformConstants CBData{OBJ->Transform.ModelMatrix};
         //void *mapped = nullptr;
       /* Data.TransformBuffer_Upload->Map(0, nullptr, &mapped);
        memcpy(mapped, &CBData, sizeof(CBData));
        Data.TransformBuffer_Upload->Unmap(0, nullptr);*/
        //memcpy(Data.MappedPtr, &CBData, sizeof(CBData));

        CmdList->SetGraphicsRootConstantBufferView(1, Data.GPUAddress);
        OBJ->GPUMesh->Bind(CmdList);
        CmdList->DrawIndexedInstanced(OBJ->GPUMesh->GetIndexCount(), 1, 0, 0, 0);
    }
}



void Renderer::OnResize(float NewAspectRatio) { Camera.SetLens(DirectX::XM_PIDIV4, NewAspectRatio, 0.1f, 1000.0f); }