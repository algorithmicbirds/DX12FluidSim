#include "../ParticleSystem/Particle_cs.hlsl"

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
    float2 uv : TEXCOORD;
};

//StructuredBuffer<Particle> gParticlesVS : register(t0);

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;

    float2 triVerts[3] =
    {
        float2(-1.0f, -1.0f), 
        float2(-1.0f, 3.0f),
        float2(3.0f, -1.0f), 
    };

    output.Position = float4(triVerts[vertexID], 0.0f, 1.0f);
    
    output.Color = float3(1, 0.5, 0);
    // Map NDC [-1, -1] to UV [0, 1]
    output.uv = 0.5f * (triVerts[vertexID] + float2(1.0f, 1.0f));
    return output;
}

