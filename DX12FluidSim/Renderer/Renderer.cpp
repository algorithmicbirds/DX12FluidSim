#include "Renderer/Renderer.hpp"
#include "D3D/DXSwapchain.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "D3D/DXGraphicsPipeline.hpp"
#include <iostream>
#include "Shared/Utils.hpp"
#include "FluidPipelines/FluidForcesComputePipeline.hpp"
#include "Shared/RootSignature.hpp"
#include "Shared/RootParams.hpp"
#include "FluidPipelines/FluidPipelinesHeapDesc.hpp"
#include "FluidPipelines/FluidIntegrateComputePipeline.hpp"
#include "FluidPipelines/HashComputePipeline.hpp"
#include "FluidPipelines/BuildGridComputePipeline.hpp"

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
    RunParticlesHashComputePipeline(CmdList);
    RunParticleGridComputePipeline(CmdList);
    RunParticlesForcesComputePipeline(CmdList);
    RunParticlesIntegrateComputePipeline(CmdList);
    RunParticlesGraphicsPipeline(CmdList);
    // RunDensityVisualizationGraphicsPipeline(CmdList);
    RunBoundingBoxGraphicsPipeline(CmdList);
}

void Renderer::ClearFrame(ID3D12GraphicsCommandList7 *CmdList)
{
    float ClearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};

    CmdList->ClearRenderTargetView(SwapchainRef.GetMSAARTVHandle(), ClearColor, 0, nullptr);
    CmdList->OMSetRenderTargets(1, &SwapchainRef.GetMSAARTVHandle(), FALSE, &SwapchainRef.GetMSAADSVHandle());
    CmdList->ClearDepthStencilView(SwapchainRef.GetMSAADSVHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void Renderer::RunParticlesHashComputePipeline(ID3D12GraphicsCommandList7 *CmdList)
{
    ParticleHashComputePipeline->BindRootAndPSO(CmdList);
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::PrecomputedKernalCB_b3, ParticleBuffer.GPUAddress);
    ID3D12DescriptorHeap *Heaps[] = {FluidHeapDesc->GetDescriptorHeap()};
    CmdList->SetDescriptorHeaps(1, Heaps);
    
    D3D12_GPU_DESCRIPTOR_HANDLE ForcesSRV = bPingPong
                                                ? ParticleIntegrateComputePipeline->GetParticleIntegrateSRVGPUHandle()
                                                : ParticleForcesComputePipeline->GetParticleForcesSRVGPUHandle();

    CmdList->SetComputeRootDescriptorTable(ComputeRootParams::ParticlePrevPositionsSRV_t1, ForcesSRV);

    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::ParticleHashUAV_u1, ParticleHashComputePipeline->GetHashUAVGPUHandle()
    );

    DispatchComputeWithBarrier(CmdList, ParticleHashComputePipeline->GetHashBuffer());
    ParticleHashComputePipeline->ReadBackHashBuffer(CmdList);
    ParticleHashComputePipeline->SortHashedValues();
}


void Renderer::RunParticleGridComputePipeline(ID3D12GraphicsCommandList7 *CmdList)
{
    ParticleGridComputePipeline->BindRootAndPSO(CmdList);
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::PrecomputedKernalCB_b3, ParticleBuffer.GPUAddress);
    ID3D12DescriptorHeap *Heaps[] = {FluidHeapDesc->GetDescriptorHeap()};
    CmdList->SetDescriptorHeaps(1, Heaps);
    /*CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::SortedHashSRV_t3, ParticleSortComputePipeline->GetBitonicSortSRVGPUHandle()
    );*/
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::SortedHashSRV_t3, ParticleGridComputePipeline->GetCellStartSRVGPUHandle()
    );
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::CellStartUAV_u3, ParticleGridComputePipeline->GetCellStartUAVGPUHandle()
    );
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::CellEndUAV_u4, ParticleGridComputePipeline->GetCellEndUAVGPUHandle()
    );

    UINT ThreadGroupSize = 256;
    UINT NumGroups = (ParticleCount + ThreadGroupSize - 1) / ThreadGroupSize;
    CmdList->Dispatch(NumGroups, 1, 1);

    D3D12_RESOURCE_BARRIER CellStartBR{};
    CellStartBR.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    CellStartBR.UAV.pResource = ParticleGridComputePipeline->GetCellStartBuffer();
    CmdList->ResourceBarrier(1, &CellStartBR);

    D3D12_RESOURCE_BARRIER CellEndBR{};
    CellEndBR.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    CellEndBR.UAV.pResource = ParticleGridComputePipeline->GetCellEndBuffer();
    CmdList->ResourceBarrier(1, &CellEndBR);
}

void Renderer::RunParticlesForcesComputePipeline(ID3D12GraphicsCommandList7 *CmdList)
{
    ParticleForcesComputePipeline->BindRootAndPSO(CmdList);
    CmdList->SetComputeRootConstantBufferView(
        ComputeRootParams::TimerCB_b0, ConstantBuffersRef.GetTimerGPUVirtualAddress()
    );
    CmdList->SetComputeRootConstantBufferView(
        ComputeRootParams::BoundingBoxCB_b1, ConstantBuffersRef.GetBoundingBoxGPUVirtualAddress()
    );
    CmdList->SetComputeRootConstantBufferView(
        ComputeRootParams::ComputeSimParamsCB_b2, ConstantBuffersRef.GetComputeSimParamsGPUVirtualAddress()
    );
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::PrecomputedKernalCB_b3, ParticleBuffer.GPUAddress);
    ID3D12DescriptorHeap *Heaps[] = {FluidHeapDesc->GetDescriptorHeap()};
    CmdList->SetDescriptorHeaps(1, Heaps);

    D3D12_GPU_DESCRIPTOR_HANDLE ForcesSRV = bPingPong
                                                ? ParticleIntegrateComputePipeline->GetParticleIntegrateSRVGPUHandle()
                                                : ParticleForcesComputePipeline->GetParticleForcesSRVGPUHandle();

    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::ParticleForcesUAV_u0, ParticleForcesComputePipeline->GetParticleForcesUAVGPUHandle()
    );
    CmdList->SetComputeRootDescriptorTable(ComputeRootParams::DebugUAV_u5, DebugBuffer.GetDebugGPUDescHandle());

    CmdList->SetComputeRootDescriptorTable(ComputeRootParams::ParticlePrevPositionsSRV_t1, ForcesSRV);
    //CmdList->SetComputeRootDescriptorTable(
    //    ComputeRootParams::SortedHashSRV_t3, ParticleSortComputePipeline->GetBitonicSortSRVGPUHandle()
    //);
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::SortedHashSRV_t3, ParticleGridComputePipeline->GetCellStartSRVGPUHandle()
    );
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::CellStartSRV_t4, ParticleGridComputePipeline->GetCellStartSRVGPUHandle()
    );
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::CellEndSRV_t5, ParticleGridComputePipeline->GetCellEndSRVGPUHandle()
    );
    DispatchComputeWithBarrier(CmdList, ParticleForcesComputePipeline->GetParticleForcesBuffer());
    //DebugBuffer.ReadBackDebugBuffer(CmdList);
}

void Renderer::RunParticlesIntegrateComputePipeline(ID3D12GraphicsCommandList7 *CmdList)
{
    ParticleIntegrateComputePipeline->BindRootAndPSO(CmdList);
    CmdList->SetComputeRootConstantBufferView(
        ComputeRootParams::TimerCB_b0, ConstantBuffersRef.GetTimerGPUVirtualAddress()
    );
    CmdList->SetComputeRootConstantBufferView(
        ComputeRootParams::ComputeSimParamsCB_b2, ConstantBuffersRef.GetComputeSimParamsGPUVirtualAddress()
    );
    CmdList->SetComputeRootConstantBufferView(ComputeRootParams::PrecomputedKernalCB_b3, ParticleBuffer.GPUAddress);
    ID3D12DescriptorHeap *Heaps[] = {FluidHeapDesc->GetDescriptorHeap()};
    CmdList->SetDescriptorHeaps(1, Heaps);
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::ParticleForcesUAV_u0, ParticleIntegrateComputePipeline->GetParticleIntegrateUAVGPUHandle()
    );
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::ParticleForcesSRV_t0, ParticleForcesComputePipeline->GetParticleForcesSRVGPUHandle()
    );
    /*CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::SortedHashSRV_t3, ParticleSortComputePipeline->GetBitonicSortSRVGPUHandle()
    );*/
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::SortedHashSRV_t3, ParticleGridComputePipeline->GetCellStartSRVGPUHandle()
    );
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::CellStartSRV_t4, ParticleGridComputePipeline->GetCellStartSRVGPUHandle()
    );
    CmdList->SetComputeRootDescriptorTable(
        ComputeRootParams::CellEndSRV_t5, ParticleGridComputePipeline->GetCellEndSRVGPUHandle()
    );

    DispatchComputeWithBarrier(CmdList, ParticleIntegrateComputePipeline->GetParticleIntegrateBuffer());
    bPingPong = !bPingPong;
}

void Renderer::DispatchComputeWithBarrier(ID3D12GraphicsCommandList7 *CmdList, ID3D12Resource2 *Buffer)
{
    UINT ThreadGroupSize = 256;
    UINT NumGroups = (ParticleCount + ThreadGroupSize - 1) / ThreadGroupSize;
    CmdList->Dispatch(NumGroups, 1, 1);

    D3D12_RESOURCE_BARRIER Barrier{};
    Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    Barrier.UAV.pResource = Buffer;
    CmdList->ResourceBarrier(1, &Barrier);
}

void Renderer::RunParticlesGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList)
{
    ParticleGraphicsPipeline->BindRootAndPSO(CmdList);
    CmdList->SetGraphicsRootConstantBufferView(
        GraphicsRootParams::CameraCB_b0, ConstantBuffersRef.GetCameraGPUVirtualAddress()
    );
    CmdList->SetGraphicsRootConstantBufferView(
        GraphicsRootParams::TimerCB_b2, ConstantBuffersRef.GetBoundingBoxGPUVirtualAddress()
    );
    CmdList->SetGraphicsRootConstantBufferView(
        GraphicsRootParams::GraphicsSimParams_b4, ConstantBuffersRef.GetGraphicsSimParamsGPUVirtualAddress()
    );
    ID3D12DescriptorHeap *Heaps[] = {FluidHeapDesc->GetDescriptorHeap()};
    CmdList->SetDescriptorHeaps(1, Heaps);
    CmdList->SetGraphicsRootDescriptorTable(
        GraphicsRootParams::ParticleForcesSRV_t0, ParticleIntegrateComputePipeline->GetParticleIntegrateSRVGPUHandle()
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
    CmdList->SetGraphicsRootConstantBufferView(
        GraphicsRootParams::CameraCB_b0, ConstantBuffersRef.GetCameraGPUVirtualAddress()
    );
    CmdList->SetGraphicsRootConstantBufferView(
        GraphicsRootParams::TimerCB_b2, ConstantBuffersRef.GetTimerGPUVirtualAddress()
    );
    CmdList->SetGraphicsRootConstantBufferView(
        GraphicsRootParams::GraphicsSimParams_b4, ConstantBuffersRef.GetGraphicsSimParamsGPUVirtualAddress()
    );
    ID3D12DescriptorHeap *Heaps[] = {FluidHeapDesc->GetDescriptorHeap()};
    CmdList->SetDescriptorHeaps(_countof(Heaps), Heaps);
    CmdList->SetGraphicsRootDescriptorTable(
        GraphicsRootParams::ParticleForcesSRV_t0, ParticleIntegrateComputePipeline->GetParticleIntegrateSRVGPUHandle()
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
    CmdList->SetGraphicsRootConstantBufferView(
        GraphicsRootParams::CameraCB_b0, ConstantBuffersRef.GetCameraGPUVirtualAddress()
    );
    CmdList->SetGraphicsRootConstantBufferView(
        GraphicsRootParams::BoundingBoxCB_b3, ConstantBuffersRef.GetBoundingBoxGPUVirtualAddress()
    );
    CmdList->RSSetViewports(1, &Viewport);
    CmdList->RSSetScissorRects(1, &SwapchainRef.GetScissorRect());
    constexpr UINT BoxVertices = 5;
    CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
    CmdList->DrawInstanced(BoxVertices, 1, 0, 0);
}

void Renderer::InitializeBuffers(ID3D12GraphicsCommandList7 *CmdList)
{
    InitalizeComputePipelines(CmdList);
    UINT PrecompParticleCosnstBufferSize = sizeof(PrecomputedParticleConstants);
    PrecomputedParticleConstants PrecompParticleData{};
    // calculates 2d kernal poly6
    const float Poly6SmoothingRadiusPow8 = pow(ParticleInitialValues::ParticleSmoothingRadius, 8);
    PrecompParticleData.Poly6SmoothingRadiusSquared = pow(ParticleInitialValues::ParticleSmoothingRadius, 2);
    PrecompParticleData.Poly6KernelConst = 4.0f / (PI * Poly6SmoothingRadiusPow8);

    // 2d spiky graident
    const float SpikySmoothingRadPow5 = pow(ParticleInitialValues::ParticleSmoothingRadius, 5);
    PrecompParticleData.SpikyKernelConst = -(30.0f / (PI * SpikySmoothingRadPow5));

    PrecompParticleData.ParticleCount = ParticleCount;
    PrecompParticleData.GridCellSize = SimInitials::GridCellSize;
    PrecompParticleData.HashTableSize = SimInitials::HashTableSize;
    // creates global data that is shared between both pipelines
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

void Renderer::InitalizeComputePipelines(ID3D12GraphicsCommandList7 *CmdList)
{
    FluidHeapDesc = std::make_unique<FluidHeapDescriptor>(DeviceRef);

    ComPtr<ID3D12RootSignature> CompRootSig = RootSignature::CreateComputeRootSig(DeviceRef);

    ParticleForcesComputePipeline = CreateComputePipelineInstance<FluidForcesComputePipeline>(
        DeviceRef, CompRootSig, CmdList, SHADER_PATH "ParticleSystem/ParticleForces_cs.cso", *FluidHeapDesc.get()
    );

    ParticleIntegrateComputePipeline = CreateComputePipelineInstance<FluidIntegrateComputePipeline>(
        DeviceRef, CompRootSig, CmdList, SHADER_PATH "ParticleSystem/ParticleIntegrate_cs.cso", *FluidHeapDesc.get()
    );

    ParticleHashComputePipeline = CreateComputePipelineInstance<HashComputePipeline>(
        DeviceRef, CompRootSig, CmdList, SHADER_PATH "ParticleSystem/ParticleHash_cs.cso", *FluidHeapDesc.get()
    );

  /*  ParticleSortComputePipeline = CreateComputePipelineInstance<BitonicSortComputePipeline>(
        DeviceRef, CompRootSig, CmdList, SHADER_PATH "ParticleSystem/BitonicSort_cs.cso", *FluidHeapDesc.get()
    );*/

    ParticleGridComputePipeline = CreateComputePipelineInstance<BuildGridComputePipeline>(
        DeviceRef, CompRootSig, CmdList, SHADER_PATH "ParticleSystem/ParticleGrid_cs.cso", *FluidHeapDesc.get()
    );

    DebugBuffer.CreateDebugUAVDesc(DeviceRef, *FluidHeapDesc.get());
}