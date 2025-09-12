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
    SimParamsCPU.Gravity = Gravity;
    UpdateSimParamsData();
}

void ConstantBuffers::SetCollisionDampingData(float CollisionDamping)
{
    SimParamsCPU.Damping = CollisionDamping;
    UpdateSimParamsData();
}

void ConstantBuffers::SetPauseToggle(UINT PauseToggle)
{
    SimParamsCPU.Pause = PauseToggle;
    UpdateSimParamsData();
}

void ConstantBuffers::SetHeightAndWidth(float Height, float Width)
{
    ScreenConstCPU.ScreenSize = {Width, Height};
    ScreenCB.Update(ScreenConstCPU);
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
    //Camera.SetLens(DirectX::XM_PIDIV4, AspectRatio, 0.1f, 1000.0f);

    SimParamsCB.Initialize(Device);
    TimerCB.Initialize(Device);
    CameraCB.Initialize(Device);
    BoundingBoxCB.Initialize(Device);
    ScreenCB.Initialize(Device);
    ScreenCB.Update(ScreenConstCPU);
    UpdateBoundingBoxData();
    UpdateSimParamsData();
}

void ConstantBuffers::UpdateSimParamsData() { SimParamsCB.Update(SimParamsCPU); }

void ConstantBuffers::UpdateBoundingBoxData() { BoundingBoxCB.Update(BoundingBoxCPU); }

void ConstantBuffers::OnResize(float NewAspectRatio) { Camera.SetLens(DirectX::XM_PIDIV4, NewAspectRatio, 0.1f, 1000.0f); }
