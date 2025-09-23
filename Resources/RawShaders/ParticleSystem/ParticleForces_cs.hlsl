#define PI 3.14159265f

struct FluidParticle
{
    float3 Position;
    float ParticleRadius;
    float3 Acceleration;
    float ParticleSmoothingRadius;
    float3 Velocity;
    float Density;
    float2 PressureForce;
    float Mass;
};

struct DebugParticleData
{
    float Density;
};

RWStructuredBuffer<FluidParticle> ParticlesOutputBuffer : register(u0);
RWStructuredBuffer<DebugParticleData> ParticlesDebugBuffer : register(u5);
StructuredBuffer<FluidParticle> ParticlesInputBuffer : register(t1);

struct ParticleCellHashData
{
    uint CellHashValue;
    uint ParticleIndex;
};

StructuredBuffer<ParticleCellHashData> SortedParticleHashes : register(t3);
StructuredBuffer<uint> GridCellStartIndices : register(t4);
StructuredBuffer<uint> GridCellEndIndices : register(t5);

cbuffer TimeStepData : register(b0)
{
    float DeltaTime;
}

cbuffer SimulationControlParameters : register(b2)
{
    float Gravity;
    float VelocityDamping;
    float PressureStiffnessConstant;
    int SimulationPaused;
}

cbuffer PrecomputedKernelParameters : register(b3)
{
    float Poly6KernelSmoothingRadiusSquared;
    float Poly6KernelCoefficient;
    float SpikyKernelCoefficient;
    uint TotalParticleCount;
    uint SpatialHashTableSize;
    float GridCellSize;
}

uint Compute2DSpatialHash(int2 cellCoordinates)
{
    const uint primeX = 73856093;
    const uint primeY = 19349663;
    uint hashValue = ((uint) cellCoordinates.x * primeX) ^ ((uint) cellCoordinates.y * primeY);
    return hashValue % SpatialHashTableSize;
}

float CalculatePoly6SmoothingKernel(float squaredDistance)
{
    if (squaredDistance >= Poly6KernelSmoothingRadiusSquared)
        return 0.0f;

    float weightPolynomial = Poly6KernelSmoothingRadiusSquared - squaredDistance;
    return Poly6KernelCoefficient * weightPolynomial * weightPolynomial * weightPolynomial;
}

float ComputeParticleDensity(uint particleIndex)
{
    FluidParticle particle = ParticlesInputBuffer[particleIndex];
    float2 particlePosition2D = particle.Position.xy;
    float density = 0.0f;

    int2 particleCellCoordinates = int2(floor(particlePosition2D.x / GridCellSize), floor(particlePosition2D.y / GridCellSize));

    for (int yOffset = -1; yOffset <= 1; ++yOffset)
    {
        for (int xOffset = -1; xOffset <= 1; ++xOffset)
        {
            int2 neighborCellCoordinates = particleCellCoordinates + int2(xOffset, yOffset);
            uint neighborCellHash = Compute2DSpatialHash(neighborCellCoordinates);
            uint startIndex = GridCellStartIndices[neighborCellHash];
            uint endIndex = GridCellEndIndices[neighborCellHash];
            if (startIndex >= endIndex)
                continue;

            for (uint neighborIndexInCell = startIndex; neighborIndexInCell < endIndex; ++neighborIndexInCell)
            {
                FluidParticle neighborParticle = ParticlesInputBuffer[SortedParticleHashes[neighborIndexInCell].ParticleIndex];
                float2 directionVector = particlePosition2D - neighborParticle.Position.xy;
                float distanceSquared = dot(directionVector, directionVector);
                if (distanceSquared >= particle.ParticleSmoothingRadius * particle.ParticleSmoothingRadius)
                    continue;
                density += neighborParticle.Mass * CalculatePoly6SmoothingKernel(distanceSquared);
            }
        }
    }

    return density;
}

float2 ComputeParticlePressureForce(uint particleIndex)
{
    FluidParticle particle = ParticlesInputBuffer[particleIndex];
    float2 particlePosition2D = particle.Position.xy;
    float2 pressureForce = float2(0.0f, 0.0f);
    float restDensity = 1000.0f;
    float particlePressure = PressureStiffnessConstant * (particle.Density - restDensity);

    int2 particleCellCoordinates = int2(floor(particlePosition2D.x / GridCellSize), floor(particlePosition2D.y / GridCellSize));

    for (int yOffset = -1; yOffset <= 1; ++yOffset)
    {
        for (int xOffset = -1; xOffset <= 1; ++xOffset)
        {
            int2 neighborCellCoordinates = particleCellCoordinates + int2(xOffset, yOffset);
            uint neighborCellHash = Compute2DSpatialHash(neighborCellCoordinates);
            uint startIndex = GridCellStartIndices[neighborCellHash];
            uint endIndex = GridCellEndIndices[neighborCellHash];
            if (startIndex >= endIndex)
                continue;

            for (uint neighborIndexInCell = startIndex; neighborIndexInCell < endIndex; ++neighborIndexInCell)
            {
                uint neighborParticleIndex = SortedParticleHashes[neighborIndexInCell].ParticleIndex;
                if (neighborParticleIndex == particleIndex)
                    continue;

                FluidParticle neighborParticle = ParticlesInputBuffer[neighborParticleIndex];
                float2 directionVector = particlePosition2D - neighborParticle.Position.xy;
                float distanceSquared = dot(directionVector, directionVector);
                if (distanceSquared >= particle.ParticleSmoothingRadius * particle.ParticleSmoothingRadius || distanceSquared == 0.0f)
                    continue;

                float distance = sqrt(distanceSquared);
                float neighborPressure = PressureStiffnessConstant * (neighborParticle.Density - restDensity);
                float radialFalloff = (particle.ParticleSmoothingRadius - distance);
                radialFalloff *= radialFalloff;
                float2 gradientVector = SpikyKernelCoefficient * radialFalloff * (directionVector / distance);

                pressureForce += -neighborParticle.Mass * (particlePressure + neighborPressure) / (2.0f * neighborParticle.Density) * gradientVector;
            }
        }
    }

    return pressureForce;
}

[numthreads(256, 1, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint particleIndex = dispatchThreadID.x;
    if (particleIndex >= TotalParticleCount)
        return;

    FluidParticle particle = ParticlesInputBuffer[particleIndex];
    particle.Density = ComputeParticleDensity(particleIndex);
    ParticlesDebugBuffer[particleIndex].Density = particle.Density;
    particle.PressureForce = ComputeParticlePressureForce(particleIndex);
    ParticlesOutputBuffer[particleIndex] = particle;
}
