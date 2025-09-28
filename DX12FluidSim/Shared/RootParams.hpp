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
constexpr UINT BitonicSortConstCB_b4 = 4;
constexpr UINT ParticleForcesUAV_u0 = 5;
constexpr UINT ParticleHashUAV_u1 = 6;
constexpr UINT ParticleSortUAV_u2 = 7;
constexpr UINT CellStartUAV_u3 = 8;
constexpr UINT CellEndUAV_u4 = 9;
constexpr UINT ParticleForcesSRV_t0 = 10;
constexpr UINT ParticlePrevPositionsSRV_t1 = 11;
constexpr UINT ParticleHashSRV_t2 = 12;
constexpr UINT SortedHashSRV_t3 = 13;
constexpr UINT CellStartSRV_t4 = 14;
constexpr UINT CellEndSRV_t5 = 15;
constexpr UINT DebugUAV_u5 = 16;
} // namespace ComputeRootParams
