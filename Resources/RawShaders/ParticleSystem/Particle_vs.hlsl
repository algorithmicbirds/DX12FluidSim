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
    float ParticleRadius : TEXCOORD1;
    float ParticleDensity : TEXCOORD2;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;

    uint particleID = vertexID / 6;
    uint cornerID = vertexID % 6;

    Particle p = gParticlesVS[particleID];

    float3 quadOffsets[4] =
    {
        float3(-p.ParticleRadius, p.ParticleRadius, 0), // v0
        float3(p.ParticleRadius, p.ParticleRadius, 0), // v1
        float3(-p.ParticleRadius, -p.ParticleRadius, 0), // v2
        float3(p.ParticleRadius, -p.ParticleRadius, 0) // v3
    };

    uint mapping[6] = { 0, 1, 2, 2, 1, 3 };
    float3 pos = p.Position + quadOffsets[mapping[cornerID]];

    float2 quadUVs[4] =
    {
        float2(0, 0),
        float2(1, 0),
        float2(0, 1),
        float2(1, 1)
    };
    output.Position = mul(float4(pos, 1.0f), ViewProjection);
    output.Color = float3(1, 0.5, 0);
    output.uv = quadUVs[mapping[cornerID]];
    output.ParticleDensity = p.Density;
    output.ParticleRadius = p.ParticleRadius;
    return output;
}

