struct Particle
{
    float3 Position;
};

struct ParticleHashData
{
    uint CellHash;
    uint ParticleIndex;
};

StructuredBuffer<Particle> gParticleBuffer : register(t1);

RWStructuredBuffer<ParticleHashData> gParticleHashBuffer : register(u1);

cbuffer PrecomputedKernalsData : register(b3)
{
    float Poly6SmoothingRadiusPow2;
    float Poly6KernalConst;
    float SpikyKernalConst;
    uint ParticleCount;
    uint HashTableSize;
    float GridCellSize;
}

uint Compute2DSpatialHash(int2 cellCoordinates, uint hashTableSize)
{
    const uint primeX = 73856093;
    const uint primeY = 19349663;

    uint hashValue = ((uint) cellCoordinates.x * primeX) ^ ((uint) cellCoordinates.y * primeY);
    return hashValue % hashTableSize; 
}

[numthreads(256, 1, 1)]
void CSMain(uint3 threadID : SV_DispatchThreadID)
{
    uint particleIndex = threadID.x;

    Particle currentParticle = gParticleBuffer[particleIndex];

    int2 cellCoordinates;
    cellCoordinates.x = (int) floor(currentParticle.Position.x / GridCellSize);
    cellCoordinates.y = (int) floor(currentParticle.Position.y / GridCellSize);

    uint cellHashValue = Compute2DSpatialHash(cellCoordinates, HashTableSize);

    ParticleHashData outputData;
    outputData.CellHash = cellHashValue;
    outputData.ParticleIndex = particleIndex;

    gParticleHashBuffer[particleIndex] = outputData;
}
