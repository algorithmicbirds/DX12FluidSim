#include "VisualizeDensity_vs.hlsl"

StructuredBuffer<Particle> gParticlesVS : register(t0);

cbuffer ScreenConstant : register(b4)
{
    float2 ScreenSize;
    uint PixelParticlCount; 
}

float GaussianKernel(float2 diff, float radius)
{
    return exp(-dot(diff, diff) / (2.0f * radius * radius));
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float2 fragUV = input.uv;
}
