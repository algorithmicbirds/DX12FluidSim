#pragma once

namespace GraphicsRootParams
{
constexpr UINT CameraCB_b0 = 0;
constexpr UINT ModelCB_b1 = 1;
constexpr UINT TimerCB_b2 = 2;
constexpr UINT BoundingBoxCB_b3 = 3;
constexpr UINT ParticleSRV_t0 = 4;
constexpr UINT ScreenCB_b4 = 5;
constexpr UINT DebugUAV_u0 = 6;
} // namespace GraphicsRootParams

namespace ComputeRootParams
{
constexpr UINT TimerCB_b0 = 0;
constexpr UINT BoundingBoxCB_b1 = 1;
constexpr UINT SimParamsCB_b2 = 2;
constexpr UINT PrecomputedKernalCB_b3 = 3;
constexpr UINT ScreenCB_b4 = 4;
constexpr UINT ParticleUAV_t0 = 5;
constexpr UINT DebugUAV_t1 = 6;
constexpr UINT DensityTexUAV_t2 = 7;
} // namespace ComputeRootParams
