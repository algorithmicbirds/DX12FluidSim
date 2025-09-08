#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>
#include "D3D/Vertex.hpp"
#include <memory>
#include "Renderer/Camera.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/GameObject.hpp"
#include <unordered_map>

class DXGraphicsPipeline;
class DXSwapchain;
class DXComputePipeline;

struct GameObjectGPUData
{
    ComPtr<ID3D12Resource2> TransformBuffer_Default;
    ComPtr<ID3D12Resource2> TransformBuffer_Upload;
    void *MappedPtr = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS GPUAddress;
};

struct TimerGPUData
{
    ComPtr<ID3D12Resource2> TimerBuffer_Upload;
    void *MappedPtr = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS GPUAddress;
};

struct CameraGPUData
{
    ComPtr<ID3D12Resource2> CameraBuffer_Upload;
    void *MappedPtr = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS GPUAddress;
};

struct BoundingBoxGPUData
{
    ComPtr<ID3D12Resource2> BoundingBoxBuffer_Upload;
    void *MappedPtr = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS GPUAddress;
};

struct CameraConstant
{
    DirectX::XMMATRIX ViewProjection;
};

struct TransformConstants
{
    DirectX::XMFLOAT4X4 ModelMatrix;
};

struct BoundingBoxConstant
{
    DirectX::XMFLOAT2 Min;
    DirectX::XMFLOAT2 Max;
};

struct TimerConstant
{
    float DeltaTime;
};

class Renderer
{
public:
    Renderer(DXSwapchain &Swapchain, ID3D12Device14 &Device);
    ~Renderer();
    Renderer &operator=(const Renderer &) = delete;
    Renderer(const Renderer &) = delete;

public:
    void RenderFrame(ID3D12GraphicsCommandList7 *CmdList, float DeltaTime);
    void InitializeBuffers(ID3D12GraphicsCommandList7 *CmdList);
    void RegisterGameObject(GameObject *GameObj, ID3D12GraphicsCommandList7 *CmdList);
    void OnResize(float NewAspectRatio);
    void SetViewport(D3D12_VIEWPORT NewVP) { Viewport = NewVP; }

    inline DXGraphicsPipeline *GetMeshPipeline() { return MeshPipeline.get(); }

private:
    void UpdateCameraBuffer();
    void UpdateShaderTime(float DeltaTime);
    void UpdateBoundingBoxData();
    void RenderGameObject(ID3D12GraphicsCommandList7 *CmdList);
    void ClearFrame(ID3D12GraphicsCommandList7 *CmdList);
    void RunParticlesComputePipeline(ID3D12GraphicsCommandList7 *CmdList);
    void RunParticlesGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList);
    void RunBoundingBoxGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList);
    void UpdatePerFrameData(float DeltaTime)
    {
        UpdateShaderTime(DeltaTime);
        UpdateCameraBuffer();
        UpdateBoundingBoxData();
    }

    // updated externally beware
public:
    BoundingBoxConstant BoundingBoxCPU{
        {-1.0f, -0.5625f}, // Min
        {1.0f,  0.5625f }  // Max
    };

private:
    DXSwapchain &SwapchainRef;
    ID3D12Device14 &DeviceRef;
    D3D12_VIEWPORT Viewport{};
    Camera Camera;

    std::unique_ptr<DXGraphicsPipeline> BoundingBoxPipeline;
    std::unique_ptr<DXGraphicsPipeline> MeshPipeline;
    std::unique_ptr<DXComputePipeline> ParticleComputePipeline;
    std::unique_ptr<DXGraphicsPipeline> ParticleGraphicsPipeline;

    std::unordered_map<GameObject *, GameObjectGPUData> GameObjectResources;
    std::vector<GameObject *> RegisteredObjects;

    UINT ParticleCount = 1024;

    TimerGPUData TimerData;
    CameraGPUData CameraData;
    BoundingBoxGPUData BoundingBoxData;
};