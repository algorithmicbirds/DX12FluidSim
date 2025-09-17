#define PI 3.14159265f

struct Particle
{
    float3 Position;
    float ParticleRadius;
    float3 Velocity;
    float ParticleSmoothingRadius;
    float Density;
    float2 PressureForce;
    float Mass;
};

struct DebugData
{
    float DebugDensity;
    float DebugParticleCount;
};

RWStructuredBuffer<Particle> gParticleUAV : register(u0);
StructuredBuffer<Particle> gParticleSRV : register(t0);
RWStructuredBuffer<DebugData> DebugParticles : register(u1);

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
    float StiffnessConstant;
    int Pause;
}

cbuffer PrecomputedKernalsData : register(b3)
{
    float Poly6SmoothingRadiusPow2;
    float Poly6KernalConst;
    float SpikyKernalConst;
    uint ParticleCount;
}


void ResolveCollision(inout Particle particle)
{
    float SurfaceOffset = 0.001f;
    
        
    if (particle.Position.x - particle.ParticleRadius < BBMin.x)
    {
        particle.Position.x = BBMin.x + particle.ParticleRadius + SurfaceOffset;
        particle.Velocity.x *= -Damping;
    }
    else if (particle.Position.x + particle.ParticleRadius > BBMax.x)
    {
        particle.Position.x = BBMax.x - particle.ParticleRadius - SurfaceOffset;
        particle.Velocity.x *= -Damping;
    }

    if (particle.Position.y - particle.ParticleRadius < BBMin.y)
    {
        particle.Position.y = BBMin.y + particle.ParticleRadius + SurfaceOffset;
        particle.Velocity.y *= -Damping;
    }
    else if (particle.Position.y + particle.ParticleRadius > BBMax.y)
    {
        particle.Position.y = BBMax.y - particle.ParticleRadius - SurfaceOffset;
        particle.Velocity.y *= -Damping;
    }
}

[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint i = DTid.x;
    // use particle data from previous pass
    Particle particle = gParticleSRV[i];
    
    if (Pause == 1)
    {
        particle.Velocity += float3(0.0f, -Gravity, 0.0f) * DeltaTimeCompute;
        particle.Velocity.xy += particle.PressureForce * DeltaTimeCompute / particle.Density;
        particle.Position += particle.Velocity * DeltaTimeCompute;
       
        ResolveCollision(particle);

    }

    gParticleUAV[i] = particle;
}