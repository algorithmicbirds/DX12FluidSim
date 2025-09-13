#include "VisualizeDensity_vs.hlsl"

struct PixelDebugData
{
    float DebugDensity;
};

cbuffer GraphicsSimParam : register(b4)
{
    float4 ParamParticleColor;
    float4 ParamParticleGlowColor;
}

RWStructuredBuffer<PixelDebugData> PixelDebug : register(u0);

float4 PSMain(VSOutput input) : SV_TARGET
{
    float distSq = input.uv.x * input.uv.x + input.uv.y * input.uv.y;

    if (distSq > 1.0)
        discard;

    float falloff = exp(-distSq * distSq / (input.ParticleSmoothingRad * input.ParticleSmoothingRad));

    float brightness = 1.0 - exp(-input.ParticleDensity * falloff);

    return float4(ParamParticleGlowColor.xyz * brightness, brightness);
}