#include "Particle_vs.hlsl"

cbuffer GraphicsSimParam : register(b4)
{
    float4 ParamParticleColor;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float distSq = input.uv.x * input.uv.x + input.uv.y * input.uv.y;

    if (distSq > 1.0)
        discard;

    return ParamParticleColor;
}