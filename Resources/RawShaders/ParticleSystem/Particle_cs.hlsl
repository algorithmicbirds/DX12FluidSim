struct Particle
{
    float3 Position;
    float3 Velocity;
};

RWStructuredBuffer<Particle> gParticles : register(u0);

cbuffer TimeBufferCompute : register(b0)
{
    float DeltaTimeCompute;
}

cbuffer BoundingBoxBuffer : register(b1)
{
    float2 BBMin;
    float2 BBMax;
};

[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint i = DTid.x;
    Particle p = gParticles[i];
    float ParticleRadius = 0.1f;
    float Damping = 1.0f;
    float SurfaceOffset = 0.001f;
    
    p.Velocity += float3(0.0f, -9.81f, 0.0f) * DeltaTimeCompute;
    p.Position += p.Velocity * DeltaTimeCompute;

    if (p.Position.x - ParticleRadius < BBMin.x)
    {
        p.Position.x = BBMin.x + ParticleRadius + SurfaceOffset;
        p.Velocity.x *= -Damping;
    }
    else if (p.Position.x + ParticleRadius > BBMax.x)
    {
        p.Position.x = BBMax.x - ParticleRadius - SurfaceOffset;
        p.Velocity.x *= -Damping;
    }

    if (p.Position.y - ParticleRadius < BBMin.y)
    {
        p.Position.y = BBMin.y + ParticleRadius + SurfaceOffset;
        p.Velocity.y *= -Damping;
    }
    else if (p.Position.y + ParticleRadius > BBMax.y)
    {
        p.Position.y = BBMax.y - ParticleRadius - SurfaceOffset;
        p.Velocity.y *= -Damping;
    }

    gParticles[i] = p;
}