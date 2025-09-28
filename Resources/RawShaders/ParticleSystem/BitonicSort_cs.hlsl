struct ParticleCellHash
{
    uint ParticleHash;
    uint ParticleIndex;
};


RWStructuredBuffer<ParticleCellHash> UnsortedKeys : register(u1);

cbuffer SortConstants : register(b4)
{
    uint numEntries;
    uint groupWidth;
    uint groupHeight;
    uint stepIndex;
}



[numthreads(128, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint id = DTid.x;
    uint hIndex = id & (groupWidth - 1);
    uint leftIndex = hIndex + (groupHeight + 1) * (id / groupWidth);
    uint rightStepSize = stepIndex == 0 ? groupHeight - 2 * hIndex: (groupHeight + 1) / 2;
    uint rightIndex = leftIndex + rightStepSize;
    
    if(rightIndex >= numEntries)
        return;
    
    uint rightValue = UnsortedKeys[rightIndex].ParticleHash;
    uint leftValue = UnsortedKeys[leftIndex].ParticleHash;
    
    bool SwapHappened = leftValue > rightValue;
    if(SwapHappened)
    {
        ParticleCellHash temp = UnsortedKeys[leftIndex];
        UnsortedKeys[leftIndex] = UnsortedKeys[rightIndex];
        UnsortedKeys[rightIndex] = temp;
    }
}

