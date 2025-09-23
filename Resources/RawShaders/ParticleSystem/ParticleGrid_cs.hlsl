struct ParticleHashData
{
    uint CellHash;
    uint ParticleIndex;
};

StructuredBuffer<ParticleHashData> SortedHashKeys : register(t3);
RWStructuredBuffer<uint> CellStart : register(u3);
RWStructuredBuffer<uint> CellEnd : register(u4);

cbuffer PrecomputedKernalsData : register(b3)
{
    float Poly6SmoothingRadiusPow2;
    float Poly6KernalConst;
    float SpikyKernalConst;
    uint ParticleCount;
    uint HashTableSize;
    float GridCellSize;
}

[numthreads(256, 1, 1)]
void CSMain(uint DTid : SV_DispatchThreadID)
{
    uint particleIndex = DTid.x;
    if (particleIndex >= ParticleCount)
        return;

    uint currentHellHash = SortedHashKeys[particleIndex].CellHash;
    uint previousCellHash = (particleIndex > 0) ? SortedHashKeys[particleIndex - 1].CellHash : 0xFFFFFFFF;

    if (particleIndex == 0 || previousCellHash != currentHellHash)
    {
        CellStart[currentHellHash] = particleIndex;
        if (particleIndex > 0)
        {
            CellEnd[previousCellHash] = particleIndex;
        }
    }

    if (particleIndex == ParticleCount - 1)
    {
        CellEnd[currentHellHash] = ParticleCount;
    }
}
