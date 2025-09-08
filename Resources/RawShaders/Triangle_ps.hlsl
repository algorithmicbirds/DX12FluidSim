#include "Triangle_vs.hlsl"

float4 PSMain(VSOutput input) : SV_TARGET
{
    //float2 uv = input.Position.xy / input.Position.w; 
    //uv = uv * 0.5f + 0.5f;

    //float4 color = ComputeOutput.Sample(LinearSampler, uv);
    float4 color = float4(input.Color, 1.0f);
    return color;
}
