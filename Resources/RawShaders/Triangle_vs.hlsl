struct VSOutput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

VSOutput VSMain(float2 pos : POSITION, float3 color : COLOR)
{
    VSOutput output;
    output.pos = float4(pos, 0.0f, 1.0f);
    output.color = color;
    return output;
}
