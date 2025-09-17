#include "ParticleIntegrate_cs.hlsl"

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
    float ParticleSmoothingRad : TEXCOORD3;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;

    uint particleID = vertexID / 6;
    uint cornerID = vertexID % 6;

    Particle p = gParticlesVS[particleID];
    float particleVert = p.ParticleRadius;
    
    float3 quadOffsets[4] =
    {
        float3(-particleVert, particleVert, 0),
        float3(particleVert, particleVert, 0),
        float3(-particleVert, -particleVert, 0),
        float3(particleVert, -particleVert, 0)
    };

    uint mapping[6] = { 0, 1, 2, 2, 1, 3 };
    float3 pos = p.Position + quadOffsets[mapping[cornerID]];

    float2 quadUVs[4] =
    {
        float2(-1, -1),
        float2(1, -1),
        float2(-1, 1),
        float2(1, 1)
    };
    output.Position = mul(float4(pos, 1.0f), ViewProjection);
    output.Color = float3(1, 0.5, 0);
    output.uv = quadUVs[mapping[cornerID]];
    output.ParticleDensity = p.Density;
    output.ParticleRadius = p.ParticleRadius;
    output.ParticleSmoothingRad = p.ParticleSmoothingRadius;
    return output;
}

