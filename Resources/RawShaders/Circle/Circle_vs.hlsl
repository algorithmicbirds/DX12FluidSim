#include "../Compute/Particle_cs.hlsl"

cbuffer CameraBuffer : register(b0)
{
    float4x4 ViewProjection;
};

cbuffer TransformBuffer : register(b1)
{
    row_major float4x4 Model;
}

StructuredBuffer<Particle> gParticlesVS : register(t0);


struct VSInput
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float2  uv : TEXCOORD;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
    float2 uv : TEXCOORD;
};

VSOutput VSMain(VSInput input, uint vertexID : SV_VertexID)
{
    VSOutput output;

    Particle p = gParticlesVS[0];

    float3 offsets[4] =
    {
        float3(-0.5, -0.5, 0),
        float3(0.5, -0.5, 0),
        float3(0.5, 0.5, 0),
        float3(-0.5, 0.5, 0)
    };

    uint cornerID = vertexID % 4;

    float3 worldPos = p.Position + offsets[cornerID];

    output.Position = mul(float4(worldPos, 1.0f), ViewProjection);
    output.Color = input.Color;
    output.uv = input.uv;

    return output;
}
