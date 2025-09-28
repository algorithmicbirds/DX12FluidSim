#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>
#include "D3D/Vertex.hpp"
#include <memory>
#include "Renderer/Camera.hpp"
#include <unordered_map>
#include "Shared/SimData.hpp"
#include "Renderer/ConstantBuffers.hpp"
#include "Renderer/StupidDebugBuffer.hpp"
#include "FluidPipelines/FluidComputePipelineBase.hpp"

class DXGraphicsPipeline;
class DXSwapchain;
class FluidForcesComputePipeline;
class FluidIntegrateComputePipeline;
class FluidHeapDescriptor;
class HashComputePipeline;
class BitonicSortComputePipeline;
class BuildGridComputePipeline;
class DebugComputePipeline;

struct PrecomputedParticleGPUData
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> UploadBuffer;
    D3D12_GPU_VIRTUAL_ADDRESS GPUAddress;
};

struct PrecomputedParticleConstants
{
    float Poly6SmoothingRadiusSquared;
    float Poly6KernelConst;
    float SpikyKernelConst;
    UINT ParticleCount;
    UINT HashTableSize;
    UINT GridCellSize ;
};

struct BitonicSortConstant
{
    UINT NumEntries;
    UINT GroupWidth;
    UINT GroupHeight;
    UINT StepIndex;
};

class Renderer
{
public:
    Renderer(DXSwapchain &Swapchain, ID3D12Device14 &Device, ConstantBuffers &ConstantBuffers);
    ~Renderer();
    Renderer &operator=(const Renderer &) = delete;
    Renderer(const Renderer &) = delete;

public:
    void RenderFrame(ID3D12GraphicsCommandList7 *CmdList, float DeltaTime);
    void InitializeBuffers(ID3D12GraphicsCommandList7 *CmdList);
    void SetViewport(D3D12_VIEWPORT NewVP) { Viewport = NewVP; }

private:
    void ClearFrame(ID3D12GraphicsCommandList7 *CmdList);
    void RunParticlesHashComputePipeline(ID3D12GraphicsCommandList7 *CmdList);
    void RunParticlesSortComputePipeline(ID3D12GraphicsCommandList7 *CmdList);
    void RunParticlesDebugComputePipeline(ID3D12GraphicsCommandList7 *CmdList);
    void RunParticleGridComputePipeline(ID3D12GraphicsCommandList7 *CmdList);
    void RunParticlesForcesComputePipeline(ID3D12GraphicsCommandList7 *CmdList);
    void DispatchComputeWithBarrier(ID3D12GraphicsCommandList7 *CmdList, ID3D12Resource2 *Buffer);
    void RunParticlesIntegrateComputePipeline(ID3D12GraphicsCommandList7 *CmdList);
    void RunParticlesGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList);
    void RunDensityVisualizationGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList);
    void RunBoundingBoxGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList);
    void InitalizeComputePipelines(ID3D12GraphicsCommandList7 *CmdList);
    void CalculateNearestPow2() { 
        NearestPow2 = 1;
        while (NearestPow2 < ParticleCount)
        {
            NearestPow2 <<= 1;
            NumStages++;
        }
    }

    template <typename T>
    std::unique_ptr<T> CreateComputePipelineInstance(
        ID3D12Device14 &Device,
        ComPtr<ID3D12RootSignature> RootSig,
        ID3D12GraphicsCommandList7 *CmdList,
        const std::string &ShaderPath,
        FluidHeapDescriptor &HeapDesc
    );

private:
    DXSwapchain &SwapchainRef;
    ID3D12Device14 &DeviceRef;
    ConstantBuffers &ConstantBuffersRef;
    
    D3D12_VIEWPORT Viewport{};
    
    UINT NumStages = 0;
    UINT NearestPow2 = 1;
    GPUConstantBuffer<BitonicSortConstant> BitonicSortGPUConst;

    std::unique_ptr<DXGraphicsPipeline> BoundingBoxPipeline;
    std::unique_ptr<FluidForcesComputePipeline> ParticleForcesComputePipeline;
    std::unique_ptr<FluidIntegrateComputePipeline> ParticleIntegrateComputePipeline;
    std::unique_ptr<HashComputePipeline> ParticleHashComputePipeline;
    std::unique_ptr<BitonicSortComputePipeline> ParticleSortComputePipeline;
    std::unique_ptr<BuildGridComputePipeline> ParticleGridComputePipeline;
    std::unique_ptr<DebugComputePipeline> ParticleDebugComputePipeline;
    std::unique_ptr<DXGraphicsPipeline> ParticleGraphicsPipeline;
    std::unique_ptr<DXGraphicsPipeline> DensityVisualizationGraphicsPipeline;
    std::unique_ptr<FluidHeapDescriptor> FluidHeapDesc;

    UINT ParticleCount = SimInitials::ParticleCount;
    PrecomputedParticleGPUData ParticleBuffer;
    StupidDebugBuffer DebugBuffer;

    bool bPingPong = false;
};

template <typename T>
inline std::unique_ptr<T> Renderer::CreateComputePipelineInstance(
    ID3D12Device14 &Device,
    ComPtr<ID3D12RootSignature> RootSig,
    ID3D12GraphicsCommandList7 *CmdList,
    const std::string &ShaderPath,
    FluidHeapDescriptor &HeapDesc
)
{
    static_assert(std::is_base_of_v<FluidComputePipelineBase, T>, "T must derive from FluidComputePipelineBase");
    auto Pipeline = std::make_unique<T>(Device);
    Pipeline->SetRootSignature(RootSig);
    Pipeline->CreateStructuredBuffer(CmdList);
    Pipeline->CreatePipeline(ShaderPath, HeapDesc);
    return Pipeline;
}
