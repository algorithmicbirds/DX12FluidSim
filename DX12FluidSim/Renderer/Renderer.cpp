#include "Renderer/Renderer.hpp"
#include "D3D/DXSwapchain.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "D3D/DXGraphicsPipeline.hpp"
#include <iostream>
#include "Shared/Utils.hpp"
#include "D3D/DXComputePipeline.hpp"
#include "Shared/RootSignature.hpp"
#include "Shared/RootParams.hpp"

#define PI 3.14159265f

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
    ParticleGraphicsPipeline->SetRootSignature(RootSignature);
    ParticleGraphicsPipeline->CreatePipeline(
        SHADER_PATH "ParticleSystem/Particle_vs.cso", SHADER_PATH "ParticleSystem/Particle_ps.cso"
    );

    DensityVisualizationGraphicsPipeline = std::make_unique<DXGraphicsPipeline>(DeviceRef);
    DensityVisualizationGraphicsPipeline->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    DensityVisualizationGraphicsPipeline->SetRootSignature(RootSignature);
    DensityVisualizationGraphicsPipeline->CreatePipeline(
        SHADER_PATH "ParticleSystem/Particle_vs.cso", SHADER_PATH "ParticleSystem/VisualizeDensity_ps.cso"
    );
}

Renderer::~Renderer() {}

void Renderer::RenderFrame(ID3D12GraphicsCommandList7 *CmdList, float DeltaTime)
{
    UpdatePerFrameData(DeltaTime);
    ClearFrame(CmdList);
    RunParticlesComputePipeline(CmdList);
    // RunParticlesGraphicsPipeline(CmdList);
    RunDensityVisualizationGraphicsPipeline(CmdList);
    RunBoundingBoxGraphicsPipeline(CmdList);
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
    ParticleComputePipeline->BindRootAndPSO(CmdList);
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::TimerCB_b0, TimerCB.GPUAddress);
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::BoundingBoxCB_b1, BoundingBoxCB.GPUAddress);
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::SimParamsCB_b2, SimParamsCB.GPUAddress);
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::PrecomputedKernalCB_b3, ParticleBuffer.GPUAddress);
    ID3D12DescriptorHeap *Heaps[] = {ParticleComputePipeline->GetDescriptorHeap()};
    CmdList->SetDescriptorHeaps(1, Heaps);
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::ParticleSRV_t0, ParticleComputePipeline->GetParticleUAVGPUHandle()
    );
    CmdList->SetComputeRootDescriptorTable(ComputeRootParams::DebugSRV_t1, ParticleComputePipeline->GetDebugUAVGPUHandle());

    UINT ThreadGroupSize = 256;
    UINT NumGroups = (ParticleCount + ThreadGroupSize - 1) / ThreadGroupSize;
    CmdList->Dispatch(NumGroups, 1, 1);

    ParticleComputePipeline->ReadDebugBuffer(CmdList);
}

void Renderer::RunParticlesGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList)
{
    ParticleGraphicsPipeline->BindRootAndPSO(CmdList);
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::CameraCB_b0, CameraCB.GPUAddress);
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::TimerCB_b2, TimerCB.GPUAddress);
    CmdList->SetGraphicsRootDescriptorTable(
        GraphicsRootParams::ParticleSRV_t0, ParticleComputePipeline->GetParticleSRVGPUHandle()
    );

    CmdList->RSSetViewports(1, &Viewport);
    CmdList->RSSetScissorRects(1, &SwapchainRef.GetScissorRect());

    CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    constexpr UINT ParticleVerts = 6;
    CmdList->DrawInstanced(ParticleVerts * ParticleCount, 1, 0, 0);
}

void Renderer::RunDensityVisualizationGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList)
{
    DensityVisualizationGraphicsPipeline->BindRootAndPSO(CmdList);
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::CameraCB_b0, CameraCB.GPUAddress);
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::TimerCB_b2, TimerCB.GPUAddress);
    CmdList->SetGraphicsRootDescriptorTable(
        GraphicsRootParams::ParticleSRV_t0, ParticleComputePipeline->GetParticleSRVGPUHandle()
    );

    CmdList->RSSetViewports(1, &Viewport);
    CmdList->RSSetScissorRects(1, &SwapchainRef.GetScissorRect());

    CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    constexpr UINT ParticleVerts = 6;
    CmdList->DrawInstanced(ParticleVerts * ParticleCount, 1, 0, 0);
}

void Renderer::RunBoundingBoxGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList)
{
    BoundingBoxPipeline->BindRootAndPSO(CmdList);
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::CameraCB_b0, CameraCB.GPUAddress);
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::BoundingBox_b3, BoundingBoxCB.GPUAddress);
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

    UINT PrecompParticleCosnstBufferSize = sizeof(PrecomputedParticleConstants);
    PrecomputedParticleConstants PrecompParticleData{};
    // this calculates 2d kernal
    const float Pol6SmoothingRadiusPow8 = pow(ParticleInitialValues::ParticleSmoothingRadius, 8);
    PrecompParticleData.Poly6SmoothingRadiusSquared = pow(ParticleInitialValues::ParticleSmoothingRadius, 2);
    PrecompParticleData.Poly6KernelConst = 4.0f / (PI * Pol6SmoothingRadiusPow8);
    printf("Poly6SmoothingRadiusPow2: %f\n", PrecompParticleData.Poly6SmoothingRadiusSquared);
    printf("Poly6KernelConst: %f\n", PrecompParticleData.Poly6KernelConst);

    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        PrecompParticleCosnstBufferSize,
        &PrecompParticleData,
        ParticleBuffer.DefaultBuffer,
        ParticleBuffer.UploadBuffer
    );
    ParticleBuffer.GPUAddress = ParticleBuffer.DefaultBuffer->GetGPUVirtualAddress();

    SimParamsCB.Initialize(DeviceRef);
    TimerCB.Initialize(DeviceRef);
    CameraCB.Initialize(DeviceRef);
    BoundingBoxCB.Initialize(DeviceRef);
    UpdateBoundingBoxData();
    UpdateSimParamsData();
}

void Renderer::UpdateCameraBuffer()
{
    CameraConstant Data;
    Data.ViewProjection = DirectX::XMMatrixTranspose(Camera.GetViewProjection());
    CameraCB.Update(Data);
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
        OBJ->Pipeline->BindRootAndPSO(CmdList);
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
    TimerCB.Update(Timer);
}

void Renderer::UpdateBoundingBoxData() { BoundingBoxCB.Update(BoundingBoxCPU); }

void Renderer::OnResize(float NewAspectRatio) { Camera.SetLens(DirectX::XM_PIDIV4, NewAspectRatio, 0.1f, 1000.0f); }

void Renderer::SetBoundingBoxHeight(float Height)
{
    BoundingBoxCPU.Min.y = -Height;
    BoundingBoxCPU.Max.y = Height;
    UpdateBoundingBoxData();
}

void Renderer::SetBoundingBoxWidth(float Width)
{
    BoundingBoxCPU.Min.x = -Width;
    BoundingBoxCPU.Max.x = Width;
    UpdateBoundingBoxData();
}

void Renderer::SetGravityData(float Gravity)
{
    SimParamsCPU.Gravity = Gravity;
    UpdateSimParamsData();
}

void Renderer::SetCollisionDampingData(float CollisionDamping)
{
    SimParamsCPU.Damping = CollisionDamping;
    UpdateSimParamsData();
}

void Renderer::SetPauseToggle(UINT PauseToggle)
{
    SimParamsCPU.Pause = PauseToggle;
    UpdateSimParamsData();
}

void Renderer::UpdateSimParamsData() { SimParamsCB.Update(SimParamsCPU); }