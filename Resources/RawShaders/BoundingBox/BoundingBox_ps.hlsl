#include "BoundingBox_vs.hlsl"

float4 PSMain(VSOutput input) : SV_TARGET
{
    float4 color = float4(input.Color, 1.0f);
    return color;
}
