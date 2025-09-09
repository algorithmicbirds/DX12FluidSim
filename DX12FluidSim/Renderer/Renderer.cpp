#include "Renderer/Renderer.hpp"
#include "D3D/DXSwapchain.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "D3D/DXGraphicsPipeline.hpp"
#include <iostream>
#include "Shared/Utils.hpp"
#include "D3D/DXComputePipeline.hpp"
#include "Shared/RootSignature.hpp"
#include "Shared/RootParams.hpp"

Renderer::Renderer(DXSwapchain &Swapchain, ID3D12Device14 &Device) : SwapchainRef(Swapchain), DeviceRef(Device)
{
    ComPtr<ID3D12RootSignature> RootSignature = RootSignature::CreateGraphicsRootSig(Device);
    MeshPipeline = std::make_unique<DXGraphicsPipeline>(DeviceRef);
    MeshPipeline->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    MeshPipeline->SetRootSignature(RootSignature);
    MeshPipeline->CreatePipeline(SHADER_PATH "Triangle_vs.cso", SHADER_PATH "Triangle_ps.cso");

    BoundingBoxPipeline = std::make_unique<DXGraphicsPipeline>(DeviceRef);
    BoundingBoxPipeline->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
    BoundingBoxPipeline->SetRootSignature(RootSignature);
    BoundingBoxPipeline->CreatePipeline(
        SHADER_PATH "BoundingBox/BoundingBox_vs.cso", SHADER_PATH "BoundingBox/BoundingBox_ps.cso"
    );

    ParticleGraphicsPipeline = std::make_unique<DXGraphicsPipeline>(DeviceRef);
    ParticleGraphicsPipeline->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    ParticleGraphicsPipeline->SetRootSignature(RootSignature::CreateGraphicsRootSig(Device));
    ParticleGraphicsPipeline->CreatePipeline(
        SHADER_PATH "ParticleSystem/Particle_vs.cso", SHADER_PATH "ParticleSystem/Particle_ps.cso"
    );
}

Renderer::~Renderer() {}

void Renderer::RenderFrame(ID3D12GraphicsCommandList7 *CmdList, float DeltaTime)
{
    UpdatePerFrameData(DeltaTime);
    ClearFrame(CmdList);
    RunParticlesComputePipeline(CmdList);
    RunParticlesGraphicsPipeline(CmdList);
    RunBoundingBoxGraphicsPipeline(CmdList);
    // RenderGameObject(CmdList);
}

void Renderer::ClearFrame(ID3D12GraphicsCommandList7 *CmdList)
{
    float ClearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    CmdList->ClearRenderTargetView(SwapchainRef.GetCurrentRTVHandle(), ClearColor, 0, nullptr);
    CmdList->OMSetRenderTargets(1, &SwapchainRef.GetCurrentRTVHandle(), FALSE, &SwapchainRef.GetCurrentDSVHandle());
    CmdList->ClearDepthStencilView(SwapchainRef.GetCurrentDSVHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void Renderer::RunParticlesComputePipeline(ID3D12GraphicsCommandList7 *CmdList)
{
    ParticleComputePipeline->Dispatch(CmdList);
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::TimerCB_b0, TimerData.GPUAddress);
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::BoundingBoxCB_b1, BoundingBoxData.GPUAddress);
    ID3D12DescriptorHeap *Heaps[] = {ParticleComputePipeline->GetDescriptorHeap()};
    CmdList->SetDescriptorHeaps(1, Heaps);
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::ParticleSRV_t0, ParticleComputePipeline->GetUAVGPUHandle()
    );

    UINT ThreadGroupSize = 256;
    UINT NumGroups = (ParticleCount + ThreadGroupSize - 1) / ThreadGroupSize;
    CmdList->Dispatch(NumGroups, 1, 1);
}

void Renderer::RunParticlesGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList)
{
    ParticleGraphicsPipeline->Dispatch(CmdList);
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::CameraCB_b0, CameraData.GPUAddress);
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::TimerCB_b2, TimerData.GPUAddress);
    CmdList->SetGraphicsRootDescriptorTable(
        GraphicsRootParams::ParticleSRV_t0, ParticleComputePipeline->GetSRVGPUHandle()
    );

    CmdList->RSSetViewports(1, &Viewport);
    CmdList->RSSetScissorRects(1, &SwapchainRef.GetScissorRect());

    CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    constexpr UINT ParticleVerts = 6;
    CmdList->DrawInstanced(ParticleVerts* ParticleCount, 1, 0, 0);
}

void Renderer::RunBoundingBoxGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList)
{
    BoundingBoxPipeline->Dispatch(CmdList);
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::CameraCB_b0, CameraData.GPUAddress);
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::BoundingBox_b3, BoundingBoxData.GPUAddress);
    CmdList->RSSetViewports(1, &Viewport);
    CmdList->RSSetScissorRects(1, &SwapchainRef.GetScissorRect());
    constexpr UINT BoxVertices = 5;
    CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
    CmdList->DrawInstanced(BoxVertices, 1, 0, 0);
}

void Renderer::InitializeBuffers(ID3D12GraphicsCommandList7 *CmdList)
{
    ParticleComputePipeline = std::make_unique<DXComputePipeline>(DeviceRef);
    ParticleComputePipeline->SetRootSignature(RootSignature::CreateComputeRootSig(DeviceRef));
    ParticleComputePipeline->CreateStructuredBuffer(CmdList, ParticleCount);
    ParticleComputePipeline->CreatePipeline(SHADER_PATH "ParticleSystem/Particle_cs.cso");

    Camera.SetPosition({0.0f, 0.0f, -5.0f});
    Camera.SetTarget({0.0f, 0.0f, 0.0f});
    Camera.SetLens(DirectX::XM_PIDIV4, SwapchainRef.GetAspectRatio(), 0.1f, 1000.0f);

    CameraConstant CbData;
    CbData.ViewProjection = DirectX::XMMatrixTranspose(Camera.GetViewProjection());

    UINT TimeBSize = sizeof(TimerConstant);
    Utils::CreateDynamicUploadBuffer(DeviceRef, TimeBSize, TimerData.TimerBuffer_Upload, TimerData.MappedPtr);
    TimerData.GPUAddress = TimerData.TimerBuffer_Upload->GetGPUVirtualAddress();

    UINT CbSize = (sizeof(CameraConstant) + 255) & ~255;
    Utils::CreateDynamicUploadBuffer(DeviceRef, CbSize, CameraData.CameraBuffer_Upload, CameraData.MappedPtr);
    CameraData.GPUAddress = CameraData.CameraBuffer_Upload->GetGPUVirtualAddress();

    UINT BoundingBoxSize = (sizeof(BoundingBoxConstant) + 255) & ~255;
    Utils::CreateDynamicUploadBuffer(
        DeviceRef, BoundingBoxSize, BoundingBoxData.BoundingBoxBuffer_Upload, BoundingBoxData.MappedPtr
    );
    BoundingBoxData.GPUAddress = BoundingBoxData.BoundingBoxBuffer_Upload->GetGPUVirtualAddress();
}

void Renderer::UpdateCameraBuffer()
{
    CameraConstant Data;
    Data.ViewProjection = DirectX::XMMatrixTranspose(Camera.GetViewProjection());
    memcpy(CameraData.MappedPtr, &Data, sizeof(Data));
}

void Renderer::RegisterGameObject(GameObject *GameObj, ID3D12GraphicsCommandList7 *CmdList)
{
    GameObj->Transform.UpdateMatrix();
    DirectX::XMFLOAT4X4 ModelMatrix = GameObj->Transform.ModelMatrix;
    TransformConstants CBData{GameObj->Transform.ModelMatrix};

    UINT CBSize = (sizeof(TransformConstants) + 255) & ~255;

    GameObjectGPUData Data;
    Utils::CreateUploadBuffer(
        DeviceRef, CmdList, CBSize, &CBData, Data.TransformBuffer_Default, Data.TransformBuffer_Upload
    );

    Data.GPUAddress = Data.TransformBuffer_Upload->GetGPUVirtualAddress();

    GameObjectResources[GameObj] = std::move(Data);
    RegisteredObjects.push_back(GameObj);
}

void Renderer::RenderGameObject(ID3D12GraphicsCommandList7 *CmdList)
{
    for (auto *OBJ : RegisteredObjects)
    {
        OBJ->Transform.UpdateMatrix();
        VALIDATE_PTR(OBJ->Pipeline);
        OBJ->Pipeline->Dispatch(CmdList);
        auto &Data = GameObjectResources[OBJ];
        TransformConstants CBData{OBJ->Transform.ModelMatrix};
        CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::ModelCB_b1, Data.GPUAddress);
        OBJ->GPUMesh->Bind(CmdList);
        CmdList->DrawIndexedInstanced(OBJ->GPUMesh->GetIndexCount(), 1, 0, 0, 0);
    }
}

void Renderer::UpdateShaderTime(float DeltaTime)
{
    TimerConstant Timer{};
    Timer.DeltaTime = DeltaTime;
    memcpy(TimerData.MappedPtr, &Timer, sizeof(Timer));
}

void Renderer::UpdateBoundingBoxData()
{
    memcpy(BoundingBoxData.MappedPtr, &BoundingBoxCPU, sizeof(BoundingBoxCPU));
}

void Renderer::OnResize(float NewAspectRatio) { Camera.SetLens(DirectX::XM_PIDIV4, NewAspectRatio, 0.1f, 1000.0f); }