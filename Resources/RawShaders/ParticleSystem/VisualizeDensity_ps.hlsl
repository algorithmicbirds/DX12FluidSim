#include "Particle_vs.hlsl"

float4 PSMain(VSOutput input) : SV_TARGET
{
    float2 uv = input.uv * 2.0f - 1.0f;
    float distSqr = dot(uv, uv);

    float alpha = saturate(1.0 - distSqr * 4.0f);

    alpha *= input.ParticleDensity * 100.0f;

    float3 finalColor = float3(alpha, alpha, alpha);

    return float4(finalColor, 1.0f);
}
