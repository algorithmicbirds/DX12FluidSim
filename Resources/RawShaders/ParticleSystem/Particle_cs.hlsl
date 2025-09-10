struct Particle
{
    float3 Position;
    float ParticleRadius;
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

cbuffer SimControls : register(b2)
{
    float Gravity;
    float Damping;
    int Pause;
}

[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint i = DTid.x;
    Particle p = gParticles[i];
    float Damping = 1.0f;
    float SurfaceOffset = 0.001f;
    
    p.Velocity += float3(0.0f, -9.81f, 0.0f) * DeltaTimeCompute;
    p.Position += p.Velocity * DeltaTimeCompute;

    if (p.Position.x - p.ParticleRadius < BBMin.x)
    {
        p.Position.x = BBMin.x + p.ParticleRadius + SurfaceOffset;
        p.Velocity.x *= -Damping;
    }
    else if (p.Position.x + p.ParticleRadius > BBMax.x)
    {
        p.Position.x = BBMax.x - p.ParticleRadius - SurfaceOffset;
        p.Velocity.x *= -Damping;
    }

    if (p.Position.y - p.ParticleRadius < BBMin.y)
    {
        p.Position.y = BBMin.y + p.ParticleRadius + SurfaceOffset;
        p.Velocity.y *= -Damping;
    }
    else if (p.Position.y + p.ParticleRadius > BBMax.y)
    {
        p.Position.y = BBMax.y - p.ParticleRadius - SurfaceOffset;
        p.Velocity.y *= -Damping;
    }

    gParticles[i] = p;
}