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

Renderer::Renderer(DXSwapchain &Swapchain, ID3D12Device14 &Device, ConstantBuffers &ConstantBuffers)
    : ConstantBuffersRef(ConstantBuffers), SwapchainRef(Swapchain), DeviceRef(Device)
{
    ComPtr<ID3D12RootSignature> RootSignature = RootSignature::CreateGraphicsRootSig(Device);

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
        SHADER_PATH "DensityVisualization/VisualizeDensity_vs.cso",
        SHADER_PATH "DensityVisualization/VisualizeDensity_ps.cso"
    );
}

Renderer::~Renderer() {}

void Renderer::RenderFrame(ID3D12GraphicsCommandList7 *CmdList, float DeltaTime)
{
    ConstantBuffersRef.UpdatePerFrameData(DeltaTime);
    ClearFrame(CmdList);
    RunParticlesComputePipeline(CmdList);
    RunParticlesGraphicsPipeline(CmdList);
    //RunDensityVisualizationGraphicsPipeline(CmdList);
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
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::TimerCB_b0, ConstantBuffersRef.GetTimerGPUVirtualAddress());
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::BoundingBoxCB_b1, ConstantBuffersRef.GetBoundingBoxGPUVirtualAddress());
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::SimParamsCB_b2, ConstantBuffersRef.GetSimParamsGPUVirtualAddress());
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::PrecomputedKernalCB_b3, ParticleBuffer.GPUAddress);
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::ScreenCB_b4, ConstantBuffersRef.GetScreenParamsGPUVirtualAddress());
    ID3D12DescriptorHeap *Heaps[] = {ParticleComputePipeline->GetDescriptorHeap()};
    CmdList->SetDescriptorHeaps(1, Heaps);
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::ParticleUAV_t0, ParticleComputePipeline->GetParticleUAVGPUHandle()
    );
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::DebugUAV_t1, ParticleComputePipeline->GetDebugUAVGPUHandle()
    );
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::DensityTexUAV_t2, ParticleComputePipeline->GetDensityTexUAVGPUHandle()
    );

    UINT ThreadGroupSize = 256;
    UINT NumGroups = (ParticleCount + ThreadGroupSize - 1) / ThreadGroupSize;
    CmdList->Dispatch(NumGroups, 1, 1);

    //ParticleComputePipeline->ReadDebugBuffer(CmdList);
}

void Renderer::RunParticlesGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList)
{
    ParticleGraphicsPipeline->BindRootAndPSO(CmdList);
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::CameraCB_b0, ConstantBuffersRef.GetCameraGPUVirtualAddress());
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::TimerCB_b2, ConstantBuffersRef.GetBoundingBoxGPUVirtualAddress());
    ID3D12DescriptorHeap *Heaps[] = {ParticleComputePipeline->GetDescriptorHeap()};
    CmdList->SetDescriptorHeaps(1, Heaps);
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
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::CameraCB_b0, ConstantBuffersRef.GetCameraGPUVirtualAddress());
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::TimerCB_b2, ConstantBuffersRef.GetTimerGPUVirtualAddress());
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::ScreenCB_b4, ConstantBuffersRef.GetScreenParamsGPUVirtualAddress());
    ID3D12DescriptorHeap *Heaps[] = {ParticleComputePipeline->GetDescriptorHeap()};
    CmdList->SetDescriptorHeaps(_countof(Heaps), Heaps);
    CmdList->SetGraphicsRootDescriptorTable(
        GraphicsRootParams::ParticleSRV_t0, ParticleComputePipeline->GetParticleSRVGPUHandle()
    );
    CmdList->SetGraphicsRootDescriptorTable(
        GraphicsRootParams::DensityTexSRV_t1, ParticleComputePipeline->GetDesnsityTexSRVGPUHandle()
    );
  /*  CmdList->SetGraphicsRootDescriptorTable(
        GraphicsRootParams::DebugUAV_u0, DebugGPUDescHandle
    );*/
    CmdList->RSSetViewports(1, &Viewport);
    CmdList->RSSetScissorRects(1, &SwapchainRef.GetScissorRect());

    CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    constexpr UINT ParticleVerts = 3;
    CmdList->DrawInstanced(ParticleVerts, 1, 0, 0);
}

void Renderer::RunBoundingBoxGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList)
{
    BoundingBoxPipeline->BindRootAndPSO(CmdList);
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::CameraCB_b0, ConstantBuffersRef.GetCameraGPUVirtualAddress());
    CmdList->SetGraphicsRootConstantBufferView(GraphicsRootParams::BoundingBoxCB_b3, ConstantBuffersRef.GetBoundingBoxGPUVirtualAddress());
    ID3D12DescriptorHeap *Heaps[] = {ParticleComputePipeline->GetDescriptorHeap()};
    CmdList->SetDescriptorHeaps(1, Heaps);
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

    UINT PrecompParticleCosnstBufferSize = sizeof(PrecomputedParticleConstants);
    PrecomputedParticleConstants PrecompParticleData{};
    // this calculates 2d kernal
    const float Pol6SmoothingRadiusPow8 = pow(ParticleInitialValues::ParticleSmoothingRadius, 8);
    PrecompParticleData.Poly6SmoothingRadiusSquared = pow(ParticleInitialValues::ParticleSmoothingRadius, 2);
    PrecompParticleData.Poly6KernelConst = 4.0f / (PI * Pol6SmoothingRadiusPow8);
    PrecompParticleData.ParticleCount = ParticleCount;

    Utils::CreateUploadBuffer(
        DeviceRef,
        CmdList,
        PrecompParticleCosnstBufferSize,
        &PrecompParticleData,
        ParticleBuffer.DefaultBuffer,
        ParticleBuffer.UploadBuffer
    );
    ParticleBuffer.GPUAddress = ParticleBuffer.DefaultBuffer->GetGPUVirtualAddress(); 

    ConstantBuffersRef.InitializeBuffers(DeviceRef);
}
