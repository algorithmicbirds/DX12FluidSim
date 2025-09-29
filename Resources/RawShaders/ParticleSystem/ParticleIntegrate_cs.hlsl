#define PI 3.14159265f

struct Particle
{
    float3 Position;
    float ParticleRadius;
    float3 Acceleration;
    float ParticleSmoothingRadius;
    float3 Velocity;
    float Density;
    float2 PressureForce;
    float2 NearPressure;
    float Mass;
    float NearDensity;
};


RWStructuredBuffer<Particle> gParticleUAV : register(u0);
StructuredBuffer<Particle> gParticleSRV : register(t0);

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

cbuffer InteractionConstants : register(b4)
{
    float2 MousePos;
    uint LeftMBDown;
    uint RightMBDown;
    float InteractionStrength;
    float InteractionRad;
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

float CalculateSmoothingKernalPoly6(float squaredDistance)
{
    if (squaredDistance >= Poly6SmoothingRadiusPow2)
        return 0.0f;
    
        // 4/pi h^8 (h^2 - r^2)^3
    float polynomialWeight = Poly6SmoothingRadiusPow2 - squaredDistance;
    return Poly6KernalConst * polynomialWeight * polynomialWeight * polynomialWeight;
}

float3 XSPHVelocityCorrection(uint particleIndex)
{
    Particle currentParticle = gParticleSRV[particleIndex];
    float3 vXSPH = float3(0.0f, 0.0f, 0.0f);
    
    for (uint neighborIndex = 0; neighborIndex < ParticleCount; neighborIndex++)
    {
        if (neighborIndex == particleIndex)
            continue;
        Particle neighborParticle = gParticleSRV[neighborIndex];
        
        float3 dist = currentParticle.Position - neighborParticle.Position;
        float distSq = dot(dist, dist);
        
        if (distSq > Poly6SmoothingRadiusPow2)
            continue;
        
        float SmoothingKernal = CalculateSmoothingKernalPoly6(distSq);
        vXSPH += (neighborParticle.Mass / neighborParticle.Density) * (neighborParticle.Velocity - currentParticle.Velocity) * SmoothingKernal;
    }
    return vXSPH;
}

void ApplyMouseInteraction(inout Particle particle, float2 mousePos, uint leftMBDown, uint rightMBDown)
{
    mousePos.x = BBMin.x + MousePos.x * (BBMax.x - BBMin.x);
    mousePos.y = BBMin.y + MousePos.y * (BBMax.y - BBMin.y);
    float2 toMouse = mousePos - particle.Position.xy;
    float distSq = dot(toMouse, toMouse);
    if (distSq < InteractionRad * InteractionRad)
    {
        float dist = sqrt(distSq);
        float2 dir = toMouse / (dist + 1e-6f);
        float2 force = float2(0.0f, 0.0f);
        float SpikeStr = 20.0f;
        if (leftMBDown == 1)
        {
            force -= dir * InteractionStrength * SpikeStr * (1.0f - dist / InteractionRad);
        }

        if (rightMBDown == 1)
        {
            force += dir * InteractionStrength * SpikeStr * (1.0f - dist / InteractionRad);
        }
        particle.Acceleration.xy += force;
    }
}

[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint i = DTid.x;
    Particle particle = gParticleSRV[i];
    
    if (Pause == 1)
    {
        // velocity verlet integration
        particle.Acceleration = float3(0.0f, -Gravity, 0.0f);
        particle.Acceleration.xy += particle.PressureForce / particle.Density;
        ApplyMouseInteraction(particle, MousePos, LeftMBDown, RightMBDown);
        particle.Position += particle.Velocity * DeltaTimeCompute + 0.5 * particle.Acceleration * (DeltaTimeCompute * DeltaTimeCompute);
        float3 newAcceleration = float3(0.0f, -Gravity, 0.0f);
        newAcceleration.xy += particle.PressureForce / particle.Density;
        ApplyMouseInteraction(particle, MousePos, LeftMBDown, RightMBDown);
        particle.Velocity += 0.5 * (particle.Acceleration + newAcceleration) * DeltaTimeCompute;
        particle.Acceleration = newAcceleration;
        particle.Velocity += 0.05f * XSPHVelocityCorrection(i);
        ResolveCollision(particle);
    }
    gParticleUAV[i] = particle;
}