#pragma once

#include <DirectXMath.h>

namespace SimInitials
{
constexpr float BoundingBoxHeight = 2.0f;
constexpr float BoundingBoxWidth = 3.5f;
constexpr float CollisionDamping = 0.5f;	
constexpr float Gravity = 0.0f;
constexpr UINT Pause = 0;
constexpr UINT ParticleCount = 256;
constexpr UINT HashTableSize = 1 << 20;
constexpr UINT GridCellSize = 0.2f;
constexpr float StiffnessConstant = 20.0f;
constexpr DirectX::XMFLOAT4 PariticleBaseColor(0.0f, 0.1f, 1.0f, 1.0f);
constexpr DirectX::XMFLOAT4 PariticleGlowColor(0.0f, 0.1f, 1.0f, 1.0f);
} // namespace SimInitials

namespace ParticleInitialValues
{
constexpr float ParticleRadius = 0.05f;
constexpr float ParticleSmoothingRadius = 0.2f;
constexpr UINT RadixNumBits = 11;
} // namespace ParticleInitialValues