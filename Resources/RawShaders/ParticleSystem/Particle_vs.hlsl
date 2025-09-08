#include "Particle_cs.hlsl"

cbuffer CameraBuffer : register(b0)
{
    float4x4 ViewProjection;
};

cbuffer TimeBuffer : register(b2)
{
    float DeltaTime;
}

StructuredBuffer<Particle> gParticlesVS : register(t0);

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
    float2 uv : TEXCOORD;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;

    uint particleID = vertexID / 4;
    uint cornerID = vertexID % 4; 

    Particle p = gParticlesVS[particleID];

    float3 quadOffsets[4] =
    {
        float3(-0.1f, 0.1f, 0),
        float3(0.1f, 0.1f, 0),
        float3(-0.1f, -0.1f, 0),
        float3(0.1f, -0.1f, 0)
    };
    
    float2 quadUVs[4] =
    {
        float2(0, 0), 
        float2(1, 0), 
        float2(0, 1), 
        float2(1, 1) 
    };


    float3 pos = p.Position + quadOffsets[cornerID];
    output.Position = mul(float4(pos, 1.0f), ViewProjection);
    output.Color = float3(1, 0.5, 0);
    output.uv = quadUVs[cornerID];
    return output;
}
