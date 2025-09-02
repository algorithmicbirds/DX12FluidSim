#pragma once 

#include "GlobInclude/WinInclude.hpp"

class Camera
{
public:
    Camera()
        : m_Position(0.0f, 0.0f, -5.0f),
          m_Target(0.0f, 0.0f, 0.0f),    
          m_Up(0.0f, 1.0f, 0.0f),        
          m_FOVY(DirectX::XM_PIDIV4),    
          m_AspectRatio(1.0f),          
          m_NearZ(0.1f), m_FarZ(1000.0f)
    {
    }

    inline void SetPosition(const DirectX::XMFLOAT3 &Position) { m_Position = Position; }
    inline void SetTarget(const DirectX::XMFLOAT3 &Target) { m_Target = Target; }
    inline void SetLens(float FOVYDegrees, float AspectRatio, float NearZ, float FarZ) {
        m_FOVY = FOVYDegrees;
        m_AspectRatio = AspectRatio;
        m_NearZ = NearZ;
        m_FarZ = FarZ;
    };

    inline DirectX::XMMATRIX GetView() const
    {
         return DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&m_Position), DirectX::XMLoadFloat3(&m_Target), DirectX::XMLoadFloat3(&m_Up));
    };
    inline DirectX::XMMATRIX GetProjection() const { return DirectX::XMMatrixPerspectiveFovLH(m_FOVY, m_AspectRatio, m_NearZ, m_FarZ); };
    inline DirectX::XMMATRIX GetViewProjection() const { return GetView() * GetProjection();}

private:
    DirectX::XMFLOAT3 m_Position;
    DirectX::XMFLOAT3 m_Target;
    DirectX::XMFLOAT3 m_Up;

    float m_FOVY;
    float m_AspectRatio;
    float m_NearZ;
    float m_FarZ;
};
