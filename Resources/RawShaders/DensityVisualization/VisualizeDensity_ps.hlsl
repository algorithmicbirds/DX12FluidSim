#include "VisualizeDensity_vs.hlsl"

StructuredBuffer<Particle> gParticlesPS : register(t0);

cbuffer ScreenConstant : register(b4)
{
    float2 ScreenSize;
    float PixelParticleCount;
}

uint2 UVToPixelCoordinate(float2 uv)
{
    return uint2(uv * ScreenSize);
}

struct PixelDebugData
{
    float DebugDensity;
};

//RWStructuredBuffer<PixelDebugData> gPixelDebug : register(u1);

float GaussianKernel(float r, float h)
{
    float sigma = h * 0.5f; // standard deviation
    float coeff = 1.0 / (2.0 * 3.14159265 * sigma * sigma);
    return coeff * exp(-r * r / (2.0 * sigma * sigma));
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float density;
    if (PixelParticleCount > 0)
    {
       // gPixelDebug[0].DebugDensity = gParticles[0].Density;
        density = gParticlesPS[0].Density;
    }
    
    return float4(density, density, density, 1.0f);
}
