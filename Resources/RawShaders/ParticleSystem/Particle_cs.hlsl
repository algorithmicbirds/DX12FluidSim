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
RWTexture2D<float> gDensityTex : register(u2);

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
    Particle p = gParticles[i];
    float SurfaceOffset = 0.001f;
    p.Density = CalculateDensity(i);
    
    if (Pause == 1)
    {
        p.Velocity += float3(0.0f, -Gravity, 0.0f) * DeltaTimeCompute;
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
    }

    gParticles[i] = p;
}