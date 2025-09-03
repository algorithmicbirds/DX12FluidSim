cbuffer CameraBuffer : register(b0)
{
    float4x4 ViewProjection;
};

cbuffer TransformBuffer : register(b1)
{
    float4x4 Model;
}

struct VSInput
{
    float3 Position : POSITION;
    float3 Color : COLOR;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    float4 worldPos = mul(float4(input.Position, 1.0f), Model);
    output.Position = mul(worldPos, ViewProjection);
    output.Color = input.Color;
    return output;
}
