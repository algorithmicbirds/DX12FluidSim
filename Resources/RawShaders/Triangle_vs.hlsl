cbuffer CameraBuffer : register(b0)
{
    float4x4 ViewProjection;
};

cbuffer TransformBuffer : register(b1)
{
   row_major float4x4 Model;
}

struct VSInput
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
    float2 uv : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    float4 worldPos = mul(float4(input.Position, 1.0f), Model);
    output.Position = mul(worldPos, ViewProjection);
    output.Color = input.Color;
    output.uv = input.uv;
    return output;
}
