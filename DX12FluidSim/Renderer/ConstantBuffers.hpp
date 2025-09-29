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

struct InteractionConstants
{
    DirectX::XMFLOAT2 MousePos;
    UINT LeftMBDown;
    UINT RightMBDown;
    float InteractionStrength = SimInitials::InteractionStr;
};

struct ComputeSimParamsConstants
{
    float Gravity = SimInitials::Gravity;
    float Damping = SimInitials::CollisionDamping;
    float StiffnessConstant = SimInitials::StiffnessConstant;
    UINT Pause = SimInitials::Pause;
    float ViscosityCoeffecient = SimInitials::ViscosityCoeffecient;
    float RestDensity = SimInitials::RestDensity;
};

struct GraphicsSimParamsConstants
{
    DirectX::XMFLOAT4 BaseColor = SimInitials::PariticleBaseColor;
    DirectX::XMFLOAT4 GlowColor = SimInitials::PariticleGlowColor;
};

struct DebugConstantGPUData
{
    ComPtr<ID3D12Resource2> DefaultBuffer;
    ComPtr<ID3D12Resource2> ReadBackBuffer;
    D3D12_GPU_VIRTUAL_ADDRESS GPUAddress;
};

struct ColorConstant
{
    DirectX::XMFLOAT4 BaseColor;
};

class ConstantBuffers
{
public:
    ConstantBuffers();
    ~ConstantBuffers();
    ConstantBuffers &operator=(const ConstantBuffers &) = delete;
    ConstantBuffers(const ConstantBuffers &) = delete;

public:
    void SetBoundingBoxHeightAndWidth(float Height, float Width);
    void SetGravityData(float Gravity);
    void SetCollisionDampingData(float CollisionDamping);
    void SetStiffnessConstant(float StiffnessConstant);
    void SetPauseToggle(UINT PauseToggle);
    void SetViscosityCoeffecient(float ViscosityCoeffecient);
    void SetRestDensity(float RestDensity);
    void SetUpdatedBaseColor(DirectX::XMFLOAT4 Color);
    void SetUpdatedGlowColor(DirectX::XMFLOAT4 Color);
    void UpdateComputeSimParamsData();
    void UpdateCameraBuffer();
    void UpdateShaderTime(float DeltaTime);
    void InitializeBuffers(ID3D12Device14 &Device);
    void UpdatePerFrameData(float DeltaTime)
    {
        UpdateShaderTime(DeltaTime);
        UpdateCameraBuffer();
    }
    void OnResize(float NewAspectRatio);
    void OnMouseLBDown(UINT RBDown);
    void OnMouseRBDown(UINT LBDown);
    void OnMouseMove(DirectX::XMFLOAT2 MousePos);
    void SetUpdatedInteractionStr(float InteractionStr);
    void UpdateInteractionParams() { InteractionCB.Update(InteractionConstantCPU); }

public:
    D3D12_GPU_VIRTUAL_ADDRESS GetTimerGPUVirtualAddress() const { return TimerCB.GPUAddress; }
    D3D12_GPU_VIRTUAL_ADDRESS GetBoundingBoxGPUVirtualAddress() const { return BoundingBoxCB.GPUAddress; }
    D3D12_GPU_VIRTUAL_ADDRESS GetComputeSimParamsGPUVirtualAddress() const { return ComputeSimParamsCB.GPUAddress; }
    D3D12_GPU_VIRTUAL_ADDRESS GetCameraGPUVirtualAddress() const { return CameraCB.GPUAddress; }
    D3D12_GPU_VIRTUAL_ADDRESS GetGraphicsSimParamsGPUVirtualAddress() const { return GraphicsSimParamsCB.GPUAddress; }
    D3D12_GPU_VIRTUAL_ADDRESS GetInteractionGPUVirtualAddress() const { return InteractionCB.GPUAddress; }

private:
    void UpdateBoundingBoxData();

private:
    Camera Camera;

    BoundingBoxConstant BoundingBoxCPU{
        {-SimInitials::BoundingBoxWidth, -SimInitials::BoundingBoxHeight},
        {SimInitials::BoundingBoxWidth,  SimInitials::BoundingBoxHeight }
    };

    ComputeSimParamsConstants ComputeSimParamsCPU{
        SimInitials::Gravity, SimInitials::CollisionDamping, SimInitials::StiffnessConstant, SimInitials::Pause
    };
    GraphicsSimParamsConstants GraphicsSimParamsCPU{SimInitials::PariticleBaseColor, SimInitials::PariticleGlowColor};
    InteractionConstants InteractionConstantCPU;

    GPUConstantBuffer<TimerConstant> TimerCB;
    GPUConstantBuffer<CameraConstant> CameraCB;
    GPUConstantBuffer<BoundingBoxConstant> BoundingBoxCB;
    GPUConstantBuffer<ComputeSimParamsConstants> ComputeSimParamsCB;
    GPUConstantBuffer<GraphicsSimParamsConstants> GraphicsSimParamsCB;
    GPUConstantBuffer<InteractionConstants> InteractionCB;
};
