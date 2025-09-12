#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>
#include "D3D/Vertex.hpp"
#include <memory>
#include "Renderer/Camera.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/GameObject.hpp"
#include <unordered_map>
#include "Shared/SimData.hpp"

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

template <typename T> struct ConstantBuffer
{
    ComPtr<ID3D12Resource2> UploadBuffer;
    void *MappedPtr = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS GPUAddress;

    void Initialize(ID3D12Device14 &Device)
    {
        UINT BufferSize = (sizeof(T) + 255) & ~255;
        Utils::CreateDynamicUploadBuffer(Device, BufferSize, UploadBuffer, MappedPtr);
        GPUAddress = UploadBuffer->GetGPUVirtualAddress();
    }

    void Update(const T &Data) { memcpy(MappedPtr, &Data, sizeof(T)); }
};

struct PrecomputedParticleGPUData
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> UploadBuffer;
    D3D12_GPU_VIRTUAL_ADDRESS GPUAddress;
};

struct alignas(16) PrecomputedParticleConstants
{
    float Poly6SmoothingRadiusSquared;
    float Poly6KernelConst;
    UINT ParticleCount;
    float Pad;
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

struct SimParamsConstants
{
    float Gravity = -9.81f;
    float Damping = 1.0f;
    UINT Pause = 0;
};

struct DebugConstants
{
    float Density;
};

struct DebugConstantGPUData
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> ReadBackBuffer;
    D3D12_GPU_VIRTUAL_ADDRESS GPUAddress;
};

struct ScreenConstantFrag {
    DirectX::XMFLOAT2 ScreenSize;
    UINT ParticleCount;
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
    void SetBoundingBoxHeight(float Height);
    void SetBoundingBoxWidth(float Width);
    void SetGravityData(float Gravity);
    void SetCollisionDampingData(float CollisionDamping);
    void SetPauseToggle(UINT PauseToggle);
    void SetHeightAndWidth(float Height, float Width);
    inline DXGraphicsPipeline *GetMeshPipeline() { return MeshPipeline.get(); }

private:
    void UpdateSimParamsData();
    void UpdateCameraBuffer();
    void UpdateShaderTime(float DeltaTime);
    void UpdateBoundingBoxData();
    void RenderGameObject(ID3D12GraphicsCommandList7 *CmdList);
    void ClearFrame(ID3D12GraphicsCommandList7 *CmdList);
    void RunParticlesComputePipeline(ID3D12GraphicsCommandList7 *CmdList);
    void RunParticlesGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList);
    void RunDensityVisualizationGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList);
    void RunBoundingBoxGraphicsPipeline(ID3D12GraphicsCommandList7 *CmdList);
    void UpdatePerFrameData(float DeltaTime)
    {
        UpdateShaderTime(DeltaTime);
        UpdateCameraBuffer();
    }

private:
    DXSwapchain &SwapchainRef;
    ID3D12Device14 &DeviceRef;
    D3D12_VIEWPORT Viewport{};
    Camera Camera;

    std::unique_ptr<DXGraphicsPipeline> BoundingBoxPipeline;
    std::unique_ptr<DXGraphicsPipeline> MeshPipeline;
    std::unique_ptr<DXComputePipeline> ParticleComputePipeline;
    std::unique_ptr<DXGraphicsPipeline> ParticleGraphicsPipeline;
    std::unique_ptr<DXGraphicsPipeline> DensityVisualizationGraphicsPipeline;

    std::unordered_map<GameObject *, GameObjectGPUData> GameObjectResources;
    std::vector<GameObject *> RegisteredObjects;

    UINT ParticleCount = 1024;
    PrecomputedParticleGPUData ParticleBuffer;
    DebugConstants DebugConst;

    ScreenConstantFrag ScreenConstCPU{
        {1920, 1080},
        ParticleCount
    };

    BoundingBoxConstant BoundingBoxCPU{
        {-SimInitials::BoundingBoxWidth, -SimInitials::BoundingBoxHeight},
        {SimInitials::BoundingBoxWidth,  SimInitials::BoundingBoxHeight }
    };

    SimParamsConstants SimParamsCPU{SimInitials::Gravity, SimInitials::CollisionDamping, SimInitials::Pause};

    ConstantBuffer<TimerConstant> TimerCB;
    ConstantBuffer<CameraConstant> CameraCB;
    ConstantBuffer<BoundingBoxConstant> BoundingBoxCB;
    ConstantBuffer<SimParamsConstants> SimParamsCB;
    ConstantBuffer<ScreenConstantFrag> ScreenCB;
};