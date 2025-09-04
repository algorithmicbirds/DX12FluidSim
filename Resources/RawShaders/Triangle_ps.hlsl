#include "Triangle_vs.hlsl"

Texture2D<float4> ComputeOutput : register(t0);
SamplerState LinearSampler : register(s0);

float4 PSMain(VSOutput input) : SV_TARGET
{
    float2 uv = input.Position.xy / input.Position.w; 
    uv = uv * 0.5f + 0.5f;

    float4 color = ComputeOutput.Sample(LinearSampler, uv);

    return color;
}
