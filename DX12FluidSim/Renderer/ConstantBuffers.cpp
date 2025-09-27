#include "Renderer/ConstantBuffers.hpp"

ConstantBuffers::ConstantBuffers() {}

ConstantBuffers::~ConstantBuffers() {}

void ConstantBuffers::SetBoundingBoxHeight(float Height)
{
    BoundingBoxCPU.Min.y = -Height;
    BoundingBoxCPU.Max.y = Height;
    UpdateBoundingBoxData();
}

void ConstantBuffers::SetBoundingBoxWidth(float Width)
{
    BoundingBoxCPU.Min.x = -Width;
    BoundingBoxCPU.Max.x = Width;
    UpdateBoundingBoxData();
}

void ConstantBuffers::SetGravityData(float Gravity)
{
    ComputeSimParamsCPU.Gravity = Gravity;
    UpdateComputeSimParamsData();
}

void ConstantBuffers::SetCollisionDampingData(float CollisionDamping)
{
    ComputeSimParamsCPU.Damping = CollisionDamping;
    UpdateComputeSimParamsData();
}

void ConstantBuffers::SetStiffnessConstant(float StiffnessConstant) {
    ComputeSimParamsCPU.StiffnessConstant = StiffnessConstant;
    UpdateComputeSimParamsData();
}

void ConstantBuffers::SetPauseToggle(UINT PauseToggle)
{
    ComputeSimParamsCPU.Pause = PauseToggle;
    UpdateComputeSimParamsData();
}

void ConstantBuffers::SetViscosityCoeffecient(float ViscosityCoeffecient)
{
    ComputeSimParamsCPU.ViscosityCoeffecient = ViscosityCoeffecient;
    UpdateComputeSimParamsData();
}

void ConstantBuffers::SetRestDensity(float RestDensity) {
    ComputeSimParamsCPU.RestDensity = RestDensity;
    UpdateComputeSimParamsData();
}

void ConstantBuffers::SetUpdatedBaseColor(DirectX::XMFLOAT4 Color) 
{ 
    GraphicsSimParamsCPU.BaseColor = {Color.x, Color.y, Color.z, Color.w};
    GraphicsSimParamsCB.Update(GraphicsSimParamsCPU);
}

void ConstantBuffers::SetUpdatedGlowColor(DirectX::XMFLOAT4 Color)
{
    GraphicsSimParamsCPU.GlowColor = {Color.x, Color.y, Color.z, Color.w};
    GraphicsSimParamsCB.Update(GraphicsSimParamsCPU);
}


void ConstantBuffers::UpdateCameraBuffer()
{
    CameraConstant Data;
    Data.ViewProjection = DirectX::XMMatrixTranspose(Camera.GetViewProjection());
    CameraCB.Update(Data);
}

void ConstantBuffers::UpdateShaderTime(float DeltaTime)
{
    TimerConstant Timer{};
    Timer.DeltaTime = DeltaTime;
    TimerCB.Update(Timer);
}

void ConstantBuffers::InitializeBuffers(ID3D12Device14 &Device)
{
    Camera.SetPosition({0.0f, 0.0f, -5.0f});
    Camera.SetTarget({0.0f, 0.0f, 0.0f});

    ComputeSimParamsCB.Initialize(Device);
    GraphicsSimParamsCB.Initialize(Device);
    TimerCB.Initialize(Device);
    CameraCB.Initialize(Device);
    BoundingBoxCB.Initialize(Device);
    GraphicsSimParamsCB.Update(GraphicsSimParamsCPU);
    UpdateBoundingBoxData();
    UpdateComputeSimParamsData();
}

void ConstantBuffers::UpdateComputeSimParamsData() { ComputeSimParamsCB.Update(ComputeSimParamsCPU); }

void ConstantBuffers::UpdateBoundingBoxData() { BoundingBoxCB.Update(BoundingBoxCPU); }

void ConstantBuffers::OnResize(float NewAspectRatio) { Camera.SetLens(DirectX::XM_PIDIV4, NewAspectRatio, 0.1f, 1000.0f); }
