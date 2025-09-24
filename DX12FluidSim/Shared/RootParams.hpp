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
constexpr UINT RadixHistogram_b4 = 4;
constexpr UINT ParticleForcesUAV_u0 = 5;
constexpr UINT ParticleHashUAV_u1 = 6;
constexpr UINT ParticleSortUAV_u2 = 7;
constexpr UINT CellStartUAV_u3 = 8;
constexpr UINT CellEndUAV_u4 = 9;
constexpr UINT RadixHistogramUAV_u5 = 10;
constexpr UINT RadixPrefixSumUAV_u6 = 11;
constexpr UINT RadixScatterUAV_u7 = 12;
constexpr UINT RadixScatterBinOffsetsUAV_u8 = 13;
constexpr UINT ParticleForcesSRV_t0 = 14;
constexpr UINT ParticlePrevPositionsSRV_t1 = 15;
constexpr UINT ParticleHashSRV_t2 = 16;
constexpr UINT SortedHashSRV_t3 = 17;
constexpr UINT CellStartSRV_t4 = 18;
constexpr UINT CellEndSRV_t5 = 19;
constexpr UINT RadixHistogramSRV_t6 = 20;
constexpr UINT RadixPrefixSumSRV_t7 = 21;
constexpr UINT RadixScatterSRV_t8 = 22;
} // namespace ComputeRootParams
