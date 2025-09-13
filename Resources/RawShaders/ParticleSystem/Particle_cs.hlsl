#define PI 3.14159265f

struct Particle
{
    float3 Position;
    float ParticleRadius;
    float3 Velocity;
    float ParticleSmoothingRadius;
    float Density;
    float Pressure;
    float Mass;
};

struct DebugData
{
    float DebugDensity;
    float DebugParticleCount;
};

RWStructuredBuffer<Particle> gParticles : register(u0);
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
    int Pause;
}

cbuffer PrecomputedKernalsData : register(b3)
{
    float Poly6SmoothingRadiusPow2;
    float Poly6KernalConst;
    uint ParticleCount;
}


float CalculateSmoothingKernalPoly6(float squaredDistance)
{
    if (squaredDistance >= Poly6SmoothingRadiusPow2)
        return 0.0f;
    
    // 4/pi h^8 (h^2 - r^2)^3
    float polynomialWeight = Poly6SmoothingRadiusPow2 - squaredDistance;
    return Poly6KernalConst * polynomialWeight * polynomialWeight * polynomialWeight;
}

float CalculateDensity(uint particleIndex)
{
    float density = 0.0f;
    Particle particle = gParticles[particleIndex];
    for (uint j = 0; j < ParticleCount; j++)
    {
        float2 distanceBetweenParticle = particle.Position.xy - gParticles[j].Position.xy;
        float distSquared = dot(distanceBetweenParticle, distanceBetweenParticle);
        float Influnce = CalculateSmoothingKernalPoly6(distSquared);
        density += gParticles[j].Mass * Influnce;
    }
    return density;
}

[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint i = DTid.x;
    Particle particle = gParticles[i];
    particle.Density = CalculateDensity(i);
    DebugParticles[i].DebugDensity = particle.Density;
    DebugParticles[i].DebugParticleCount = ParticleCount;
    
    if (Pause == 1)
    {
        particle.Velocity += float3(0.0f, -Gravity, 0.0f) * DeltaTimeCompute;
        particle.Position += particle.Velocity * DeltaTimeCompute;
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
        //resolveCollision(particle);
       
    }

    gParticles[i] = particle;
}