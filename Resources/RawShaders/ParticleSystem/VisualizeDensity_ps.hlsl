#include "Particle_vs.hlsl"

struct PixelDebugData
{
    float DebugDensity;
};

RWStructuredBuffer<PixelDebugData> PixelDebug : register(u0);

float4 PSMain(VSOutput input) : SV_TARGET
{
    float2 center = float2(0.5, 0.5);
    float dist = length(input.uv - center);
    float falloff = saturate(1.0 - dist * 2.0); 
    float alpha = falloff * saturate(input.ParticleDensity / 2);
    float3 fogColor = float3(0.8, 0.8, 0.8);
    return float4(fogColor * alpha, alpha);
}