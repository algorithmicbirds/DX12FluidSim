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
    const uint primeZ = 83492791; 
    const uint primeW = 101359783; 
    
    const uint offset = 0x40000000;
    uint x = (uint) (cellCoordinates.x + offset);
    uint y = (uint) (cellCoordinates.y + offset);
    
    uint hashValue = (x * primeX) ^ (y * primeY) ^ primeZ;
    
    hashValue ^= (hashValue >> 16);
    hashValue *= primeW;
    hashValue ^= (hashValue >> 13);
    
    uint result = (hashValue % hashTableSize);
    return (result == 0) ? 1 : result;
}

[numthreads(256, 1, 1)]
void CSMain(uint3 threadID : SV_DispatchThreadID)
{
    uint particleIndex = threadID.x;
    if (particleIndex >= ParticleCount)
        return;
    
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
