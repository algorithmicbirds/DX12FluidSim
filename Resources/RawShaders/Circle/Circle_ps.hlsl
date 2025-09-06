#include "Circle_vs.hlsl"

Texture2D<float4> ComputeOutput : register(t0);
SamplerState LinearSampler : register(s0);

float4 PSMain(VSOutput input) : SV_TARGET
{
    float2 uv = input.uv * 2.0f - 1.0f; 

    float distSq = dot(uv, uv); 
    float inside = step(distSq, 1.0f); 

    float3 circleColor = input.Color.rgb; 
    float3 bgColor = float3(0.0f, 0.0f, 0.0f);

    float3 finalRGB = lerp(bgColor, circleColor, inside);
    return float4(finalRGB, 1.0f);
}
