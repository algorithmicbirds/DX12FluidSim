#include "VisualizeDensity_vs.hlsl"

struct PixelDebugData
{
    float DebugDensity;
};

RWStructuredBuffer<PixelDebugData> PixelDebug : register(u0);

float4 PSMain(VSOutput input) : SV_TARGET
{
    float distSq = input.uv.x * input.uv.x + input.uv.y * input.uv.y;

    if (distSq > 1.0)
        discard;

    return float4(input.Color, 1.0f);
}