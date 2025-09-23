#pragma once

#include "GlobInclude/WinInclude.hpp"

namespace GraphicsRootParams
{
constexpr UINT CameraCB_b0 = 0;
constexpr UINT ModelCB_b1 = 1;
constexpr UINT TimerCB_b2 = 2;
constexpr UINT BoundingBoxCB_b3 = 3;
constexpr UINT GraphicsSimParams_b4 = 4;
constexpr UINT ParticleForcesSRV_t0 = 5;
constexpr UINT DebugUAV_u0 = 6;
} // namespace GraphicsRootParams

namespace ComputeRootParams
{
constexpr UINT TimerCB_b0 = 0;
constexpr UINT BoundingBoxCB_b1 = 1;
constexpr UINT ComputeSimParamsCB_b2 = 2;
constexpr UINT PrecomputedKernalCB_b3 = 3;
constexpr UINT ParticleForcesUAV_u0 = 4;
constexpr UINT ParticleMortonUAV_u1 = 5;
constexpr UINT ParticleSortUAV_u2 = 6;
constexpr UINT CellStartUAV_u3 = 7;
constexpr UINT CellEndUAV_u4 = 8;
constexpr UINT ParticleForcesSRV_t0 = 9;
constexpr UINT ParticlePrevPositionsSRV_t1 = 10;
constexpr UINT ParticleMortonSRV_t2 = 11;
constexpr UINT SortedMortonSRV_t3 = 12;
constexpr UINT CellStartSRV_t4 = 13;
constexpr UINT CellEndSRV_t5 = 14;
} // namespace ComputeRootParams
