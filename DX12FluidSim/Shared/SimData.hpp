#pragma once

namespace SimInitials
{
constexpr float BoundingBoxHeight = 2.0f;
constexpr float BoundingBoxWidth = 3.5f;
constexpr float CollisionDamping = 1.0f;
constexpr float Gravity = 0.9f;
constexpr UINT Pause = 0;
constexpr UINT ParticleCount = 256;
} // namespace SimInitials

namespace ParticleInitialValues
{
constexpr float ParticleRadius = 0.05f;
// particle smoothing radius determine how much particlese usually it can influence so going with 4 particles
constexpr float ParticleSmoothingRadius = 0.15f;
} // namespace ParticleInitialValues