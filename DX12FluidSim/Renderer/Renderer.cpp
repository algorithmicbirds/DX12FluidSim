#include "Renderer/Renderer.hpp"
#include "D3D/DXSwapchain.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "D3D/DXGraphicsPipeline.hpp"
#include <iostream>
#include "GlobInclude/Utils.hpp"
#include "D3D/DXComputePipeline.hpp"
#include "Renderer/RootSignature.hpp"
#include "GlobInclude/RootParams.hpp"

Renderer::Renderer(DXSwapchain &Swapchain, ID3D12Device14 &Device) : SwapchainRef(Swapchain), DeviceRef(Device)
{
    CirclePipeline = std::make_unique<DXGraphicsPipeline>(DeviceRef);
    CirclePipeline->SetRootSignature(RootSignature::CreateGraphicsRootSig(Device));
    CirclePipeline->CreatePipeline(SHADER_PATH "Circle/Circle_vs.cso", SHADER_PATH "Circle/Circle_ps.cso");

    MeshPipeline = std::make_unique<DXGraphicsPipeline>(DeviceRef);
    MeshPipeline->SetRootSignature(RootSignature::CreateGraphicsRootSig(Device));
    MeshPipeline->CreatePipeline(SHADER_PATH "Triangle_vs.cso", SHADER_PATH "Triangle_ps.cso");

    ParticleGraphicsPipeline = std::make_unique<DXGraphicsPipeline>(DeviceRef);
    ParticleGraphicsPipeline->SetRootSignature(RootSignature::CreateGraphicsRootSig(Device));
    ParticleGraphicsPipeline->CreatePipeline(
        SHADER_PATH "ParticleSystem/Particle_vs.cso", SHADER_PATH "ParticleSystem/Particle_ps.cso"
    );
}

Renderer::~Renderer() {}

void Renderer::RenderFrame(ID3D12GraphicsCommandList7 *CmdList, float DeltaTime)
{
    UpdatePerFrameData(DeltaTime);
    ID3D12Resource1 *CurrentBuffer = SwapchainRef.GetCurrentBackBuffer();

    Utils::TransitionResoure(CmdList, CurrentBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

    ClearFrame(CmdList);
    RunParticlesComputePipeline(CmdList);
    RunParticlesGraphicsPipeline(CmdList);

    Utils::TransitionResoure(CmdList, CurrentBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}

void Renderer::ClearFrame(ID3D12GraphicsCommandList7* CmdList) {
    float ClearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    CmdList->ClearRenderTargetView(SwapchainRef.GetCurrentRTVHandle(), ClearColor, 0, nullptr);
    CmdList->OMSetRenderTargets(1, &SwapchainRef.GetCurrentRTVHandle(), FALSE, &SwapchainRef.GetCurrentDSVHandle());
    CmdList->ClearDepthStencilView(SwapchainRef.GetCurrentDSVHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void Renderer::RunParticlesComputePipeline(ID3D12GraphicsCommandList7 *CmdList) {
    ParticleComputePipeline->Dispatch(CmdList);
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::TimerCB_b0, TimerData.GPUAddress);
    ID3D12DescriptorHeap *Heaps[] = {ParticleComputePipeline->GetDescriptorHeap()};
    CmdList->SetDescriptorHeaps(1, Heaps);
    CmdList->SetComputeRootDescriptorTable(ComputeRootParams::ParticleSRV_t0, ParticleComputePipeline->GetUAVGPUHandle());
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::TimerCB_b0, TimerData.GPUAddress);


    UINT ThreadGroupSize = 256;
    UINT NumGroups = (ParticleCount + ThreadGroupSize - 1) / ThreadGroupSize;
    CmdList->Dispatch(NumGroups, 1, 1);
}

void Renderer::RunParticlesGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList) {
    ParticleGraphicsPipeline->Dispatch(CmdList);
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::CameraCB_b0, CameraData.GPUAddress);
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::TimerCB_b2, TimerData.GPUAddress);
    CmdList->SetGraphicsRootDescriptorTable(
        GraphicsRootParams::ParticleSRV_t0, ParticleComputePipeline->GetSRVGPUHandle()
    );


    CmdList->RSSetViewports(1, &Viewport);
    CmdList->RSSetScissorRects(1, &SwapchainRef.GetScissorRect());

    CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    CmdList->DrawInstanced(ParticleCount, 1, 0, 0);
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
    /*  for (auto *OBJ : RegisteredObjects)
      {
          OBJ->Transform.UpdateMatrix();
          OBJ->Pipeline->Dispatch(CmdList);
          auto &Data = GameObjectResources[OBJ];
          TransformConstants CBData{OBJ->Transform.ModelMatrix};
          CmdList->SetGraphicsRootConstantBufferView(RootParams::ModelCB_b1, Data.GPUAddress);
          OBJ->GPUMesh->Bind(CmdList);
          CmdList->DrawIndexedInstanced(OBJ->GPUMesh->GetIndexCount(), 1, 0, 0, 0);
      }*/
}

void Renderer::UpdateShaderTime(float DeltaTime)
{
    TimerConstant Timer{};
    Timer.DeltaTime = DeltaTime;
    memcpy(TimerData.MappedPtr, &Timer, sizeof(Timer));
}


void Renderer::OnResize(float NewAspectRatio) { Camera.SetLens(DirectX::XM_PIDIV4, NewAspectRatio, 0.1f, 1000.0f); }