#pragma once

#include <DirectXMath.h>

namespace SimInitials
{
constexpr float BoundingBoxHeight = 2.0f;
constexpr float BoundingBoxWidth = 3.5f;
constexpr float CollisionDamping = 0.5f;
constexpr float Gravity = 0.0f;
constexpr UINT Pause = 0;
constexpr UINT ViscosityCoeffecient = 20.0f; 
constexpr UINT RestDensity = 600.0f; 
constexpr UINT ParticleCount = 20000;
constexpr float InteractionStr = 1.0f;
constexpr float StiffnessConstant = 20.0f;
constexpr DirectX::XMFLOAT4 PariticleBaseColor(0.0f, 0.1f, 1.0f, 1.0f);
constexpr DirectX::XMFLOAT4 PariticleGlowColor(0.0f, 0.1f, 1.0f, 1.0f);
} // namespace SimInitials

namespace ParticleInitialValues
{
constexpr float ParticleRadius = 0.01f;
constexpr float ParticleSmoothingRadius = 0.4f;
} // namespace ParticleInitialValues

struct ParticleStructuredBuffer
{
    DirectX::XMFLOAT3 Position;
    float ParticleRadius = ParticleInitialValues::ParticleRadius;
    DirectX::XMFLOAT3 Acceleration;
    float ParticleSmoothingRadius = ParticleInitialValues::ParticleSmoothingRadius;
    DirectX::XMFLOAT3 Velocity;
    float Density = 0;
    DirectX::XMFLOAT2 Pressure;
    DirectX::XMFLOAT2 NearPressure;
    float Mass = 1.0f;
    float NearDensity = 0.0f;
};