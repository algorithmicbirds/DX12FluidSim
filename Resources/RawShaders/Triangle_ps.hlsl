#include "Triangle_vs.hlsl"

float4 PSMain(VSOutput input) : SV_TARGET
{
    return float4(input.Color, 1.0f);
}
