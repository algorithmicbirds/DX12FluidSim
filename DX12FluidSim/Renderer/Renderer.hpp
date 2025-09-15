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

class DXGraphicsPipeline;
class DXSwapchain;
class DXComputePipeline;

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
};

struct DebugConstants
{
    float Density;
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
    void RunParticlesComputePipeline(ID3D12GraphicsCommandList7 *CmdList);
    void RunParticlesGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList);
    void RunDensityVisualizationGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList);
    void RunBoundingBoxGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList);

private:
    DXSwapchain &SwapchainRef;
    ID3D12Device14 &DeviceRef;
    D3D12_VIEWPORT Viewport{};

    ConstantBuffers &ConstantBuffersRef;

    std::unique_ptr<DXGraphicsPipeline> BoundingBoxPipeline;
    std::unique_ptr<DXComputePipeline> ParticleComputePipeline;
    std::unique_ptr<DXGraphicsPipeline> ParticleGraphicsPipeline;
    std::unique_ptr<DXGraphicsPipeline> DensityVisualizationGraphicsPipeline;

    UINT ParticleCount = SimInitials::ParticleCount;
    PrecomputedParticleGPUData ParticleBuffer;
    DebugConstants DebugConst;

    StupidDebugBuffer DebugBuffer;
};