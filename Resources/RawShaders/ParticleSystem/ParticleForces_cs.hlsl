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

RWStructuredBuffer<Particle> gParticlesUAV : register(u0);
StructuredBuffer<Particle> gParticlesSRV : register(t1);

cbuffer TimeBufferCompute : register(b0)
{
    float DeltaTimeCompute;
}

cbuffer SimControls : register(b2)
{
    float Gravity;
    float Damping;
    float StiffnessConstant;
    int Pause;
    float ViscosityCoeffecient;
    float RestDensity;
}

cbuffer PrecomputedKernalsData : register(b3)
{
    float Poly6SmoothingRadiusPow2;
    float Poly6KernalConst;
    float SpikyKernalConst;
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

float2 CalculatePressureForce(uint particleIndex)
{
    Particle currentParticle = gParticlesSRV[particleIndex];
    float2 pressureForce = float2(0.0f, 0.0f);
    float currentPressure = StiffnessConstant * (currentParticle.Density - RestDensity);
    float radialFalloff;
    for (uint neighborIndex = 0; neighborIndex < ParticleCount; neighborIndex++)
    {
        if (neighborIndex == particleIndex)
            continue;
        Particle neigborParticle = gParticlesSRV[neighborIndex];
        
        float2 direction = currentParticle.Position.xy - neigborParticle.Position.xy;
        float distsq = dot(direction, direction);
        if (distsq == 0 || distsq >= currentParticle.ParticleSmoothingRadius * currentParticle.ParticleSmoothingRadius)
            continue;
        
        float neigborPressure = StiffnessConstant * (neigborParticle.Density - RestDensity);
        
        float distance = sqrt(distsq);
        
        // (h-r)^2
        float radialFalloff = (currentParticle.ParticleSmoothingRadius - distance);
        radialFalloff *= radialFalloff;
        // spiky
        // -(30.0f / (PI * h^5)) * (h-r)^2 * r/|r|
        float2 gradientContrib = SpikyKernalConst * radialFalloff * (direction / distance);
        // pressure force due to particle interaction
        pressureForce += -neigborParticle.Mass * (currentPressure + neigborPressure) / (2 * neigborParticle.Density) * gradientContrib;
        
        // Monohagan's artificial viscosity
        // K * (vi - vj) · (ri - rj) / |ri - rj|^2
        float2 velocityDiff = currentParticle.Velocity.xy - neigborParticle.Velocity.xy;
        float2 smoothingKernelGradient = CalculateSmoothingKernalPoly6(distsq) * (direction / distance);
        float viscosityTerm = ViscosityCoeffecient * (dot(velocityDiff, direction) / distance);
        // contribution of viscosity to pressure
        pressureForce += -neigborParticle.Mass * viscosityTerm * smoothingKernelGradient;
    }
    
    return pressureForce;
}

float CalculateDensity(uint particleIndex)
{
    float density = 0.0f;
    Particle currentParticle = gParticlesSRV[particleIndex];
    for (uint neighborIndex = 0; neighborIndex < ParticleCount; neighborIndex++)
    {
        float2 distanceBetweenParticle = currentParticle.Position.xy - gParticlesSRV[neighborIndex].Position.xy;
        float distSquared = dot(distanceBetweenParticle, distanceBetweenParticle);
        float Influnce = CalculateSmoothingKernalPoly6(distSquared);
        density += gParticlesSRV[neighborIndex].Mass * Influnce;
    }
    return density;
}

[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint i = DTid.x;
    Particle particle = gParticlesSRV[i];
    particle.Density = CalculateDensity(i);
    particle.PressureForce = CalculatePressureForce(i);
    gParticlesUAV[i] = particle;
}