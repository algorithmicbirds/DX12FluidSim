
StructuredBuffer<uint> SortedMortonKeys : register(t3);
RWStructuredBuffer<uint> CellStart : register(u3);
RWStructuredBuffer<uint> CellEnd : register(u4);


cbuffer PrecomputedKernalsData : register(b3)
{
    float Poly6SmoothingRadiusPow2;
    float Poly6KernalConst;
    float SpikyKernalConst;
    uint ParticleCount;
}


[numthreads(256, 1, 1)]
void CSMain(uint DTid : SV_DispatchThreadID)
{
    uint particleIndex = DTid.x;
    if (particleIndex >= ParticleCount)
        return;
    
    uint MortonKey = SortedMortonKeys[particleIndex];
    if(particleIndex == 0)
    {
        CellStart[MortonKey] = 0;
    }
    else
    {
        uint previousMortonKey = SortedMortonKeys[particleIndex - 1];
        
        if(previousMortonKey != MortonKey)
        {
            CellStart[MortonKey] = particleIndex;
            CellEnd[previousMortonKey] = particleIndex;
        }
    }
    
    if(particleIndex == ParticleCount - 1)
    {
        CellEnd[MortonKey] = ParticleCount;
    }
}