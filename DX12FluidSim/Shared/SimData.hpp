#pragma once

namespace SimInitials
{
constexpr float BoundingBoxHeight = 2.0f;
constexpr float BoundingBoxWidth = 3.5f;
constexpr float CollisionDamping = 1.0f;
constexpr float Gravity = 9.8f;
constexpr UINT Pause = 0;
} // namespace SimInitials

namespace ParticleInitialValues
{
constexpr float ParticleRadius = 0.2f;
// particle smoothing radius determine how much particlese usually it can influence so going with 4 particles
constexpr float ParticleSmoothingRadius = 0.8f;
} // namespace ParticleInitialValues