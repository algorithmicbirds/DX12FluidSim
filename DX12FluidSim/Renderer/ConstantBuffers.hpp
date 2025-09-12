#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Shared/SimData.hpp"
#include "Shared/Utils.hpp"
#include "Renderer/Camera.hpp"

template <typename T> struct GPUConstantBuffer
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

struct DebugConstantGPUData
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> ReadBackBuffer;
    D3D12_GPU_VIRTUAL_ADDRESS GPUAddress;
};

struct ScreenConstantFrag
{
    DirectX::XMFLOAT2 ScreenSize;
    UINT ParticleCount;
};

class ConstantBuffers
{
public:
    ConstantBuffers();
    ~ConstantBuffers();
    ConstantBuffers &operator=(const ConstantBuffers &) = delete;
    ConstantBuffers(const ConstantBuffers &) = delete;

public:
    void SetBoundingBoxHeight(float Height);
    void SetBoundingBoxWidth(float Width);
    void SetGravityData(float Gravity);
    void SetCollisionDampingData(float CollisionDamping);
    void SetPauseToggle(UINT PauseToggle);
    void SetHeightAndWidth(float Height, float Width);
    void UpdateSimParamsData();
    void UpdateCameraBuffer();
    void UpdateShaderTime(float DeltaTime);
    void InitializeBuffers(ID3D12Device14 &Device);
    void UpdatePerFrameData(float DeltaTime)
    {
        UpdateShaderTime(DeltaTime);
        UpdateCameraBuffer();
    }
    void OnResize(float NewAspectRatio);


public:
    D3D12_GPU_VIRTUAL_ADDRESS GetTimerGPUVirtualAddress() const { return TimerCB.GPUAddress; }
    D3D12_GPU_VIRTUAL_ADDRESS GetBoundingBoxGPUVirtualAddress() const { return BoundingBoxCB.GPUAddress; }
    D3D12_GPU_VIRTUAL_ADDRESS GetSimParamsGPUVirtualAddress() const { return SimParamsCB.GPUAddress; }
    D3D12_GPU_VIRTUAL_ADDRESS GetScreenParamsGPUVirtualAddress() const { return ScreenCB.GPUAddress; }
    D3D12_GPU_VIRTUAL_ADDRESS GetCameraGPUVirtualAddress() const { return CameraCB.GPUAddress; }

private:
    void UpdateBoundingBoxData();

private:
    Camera Camera;
    ScreenConstantFrag ScreenConstCPU{
        {1920, 1080},
        1024
    };

    BoundingBoxConstant BoundingBoxCPU{
        {-SimInitials::BoundingBoxWidth, -SimInitials::BoundingBoxHeight},
        {SimInitials::BoundingBoxWidth,  SimInitials::BoundingBoxHeight }
    };

    SimParamsConstants SimParamsCPU{SimInitials::Gravity, SimInitials::CollisionDamping, SimInitials::Pause};

    GPUConstantBuffer<TimerConstant> TimerCB;
    GPUConstantBuffer<CameraConstant> CameraCB;
    GPUConstantBuffer<BoundingBoxConstant> BoundingBoxCB;
    GPUConstantBuffer<SimParamsConstants> SimParamsCB;
    GPUConstantBuffer<ScreenConstantFrag> ScreenCB;
};
