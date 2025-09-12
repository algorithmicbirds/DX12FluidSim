#pragma once

namespace GraphicsRootParams
{
constexpr UINT CameraCB_b0 = 0;
constexpr UINT ModelCB_b1 = 1;
constexpr UINT ParticleSRV_t0 = 2;
constexpr UINT TimerCB_b2 = 3;
constexpr UINT BoundingBoxCB_b3 = 4;
constexpr UINT ScreenCB_b4 = 5;
constexpr UINT DensityTexSRV_t1 = 6;
} // namespace GraphicsRootParams

namespace ComputeRootParams
{
constexpr UINT ParticleSRV_t0 = 0;
constexpr UINT TimerCB_b0 = 1;
constexpr UINT BoundingBoxCB_b1 = 2;
constexpr UINT SimParamsCB_b2 = 3;
constexpr UINT PrecomputedKernalCB_b3 = 4;
constexpr UINT DebugUAV_t1 = 5;
constexpr UINT DensityTexUAV_t2 = 6;

} // namespace ComputeRootParams
