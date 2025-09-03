#include "Renderer/Renderer.hpp"
#include "D3D/DXSwapchain.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "D3D/DXPipeline.hpp"
#include <iostream>
#include "GlobInclude/Utils.hpp"

Renderer::Renderer(DXSwapchain &Swapchain, ID3D12Device14 &Device) : SwapchainRef(Swapchain), DeviceRef(Device)
{
    Pipeline = std::make_unique<DXPipeline>(DeviceRef, SHADER_PATH "Triangle_vs.cso", SHADER_PATH "Triangle_ps.cso");
}
Renderer::~Renderer() {  }

void Renderer::BeginFrame(ID3D12GraphicsCommandList7 *CmdList)
{
    UpdateCameraBuffer();
    ID3D12Resource1 *CurrentBuffer = SwapchainRef.GetCurrentBackBuffer();
    VALIDATE_PTR(CurrentBuffer);

    Utils::TransitionResoure(CmdList, CurrentBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

    float ClearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};

    D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = SwapchainRef.GetCurrentRTVHandle();

    CmdList->ClearRenderTargetView(RTVHandle, ClearColor, 0, nullptr);
    CmdList->OMSetRenderTargets(1, &RTVHandle, false, nullptr);
    CmdList->SetPipelineState(Pipeline->GetPipelineStateObject());
    CmdList->SetGraphicsRootSignature(Pipeline->GetRootSignature());
    CmdList->SetGraphicsRootConstantBufferView(0, CameraBufferGPUAddress);
    CmdList->RSSetViewports(1, &SwapchainRef.GetViewport());
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
    TransformConstants CBData{};
    CBData.ModelMatrix = GameObj->Transform.ModelMatrix;

    UINT CBSize = (sizeof(TransformConstants) + 255) & ~255;

    GameObjectGPUData Data;
    Utils::CreateUploadBuffer(
        DeviceRef, CmdList, CBSize, &CBData, Data.TransformBuffer_Default, Data.TransformBuffer_Upload
    );

    Data.GPUAddress = Data.TransformBuffer_Default->GetGPUVirtualAddress();
    GameObjectResources[GameObj] = std::move(Data);
    RegisteredObjects.push_back(GameObj);
}

void Renderer::RenderGameObject(ID3D12GraphicsCommandList7* CmdList) {
    for (auto *OBJ : RegisteredObjects) 
    {
        OBJ->Transform.UpdateMatrix();

        auto &Data = GameObjectResources[OBJ];
        TransformConstants CBData{OBJ->Transform.ModelMatrix};

        void *mapped = nullptr;
        Data.TransformBuffer_Upload->Map(0, nullptr, &mapped);
        memcpy(mapped, &CBData, sizeof(CBData));
        Data.TransformBuffer_Upload->Unmap(0, nullptr);

        CmdList->SetGraphicsRootConstantBufferView(1, Data.GPUAddress); 
        OBJ->Mesh->Bind(CmdList);
        CmdList->DrawIndexedInstanced(OBJ->Mesh->GetIndexCount(), 1, 0, 0, 0);
    }
}
