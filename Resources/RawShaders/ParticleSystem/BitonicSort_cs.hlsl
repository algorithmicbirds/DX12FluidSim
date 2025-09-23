struct ParticleHashData
{
    uint CellHash;
    uint ParticleIndex;
};

RWStructuredBuffer<ParticleHashData> OutputSortedKeys : register(u2);
StructuredBuffer<ParticleHashData> InputKeys : register(t2);

groupshared ParticleHashData sharedKeys[256];

[numthreads(256, 1, 1)]
void CSMain(uint DTid : SV_DispatchThreadID)
{
    uint particleIndex = DTid.x;

    sharedKeys[particleIndex] = InputKeys[particleIndex];
    GroupMemoryBarrierWithGroupSync();

    for (uint sequenceLength = 2; sequenceLength <= 256; sequenceLength <<= 1)
    {
        for (uint compareDistance = sequenceLength >> 1; compareDistance > 0; compareDistance >>= 1)
        {
            uint comparePartnerIndex = particleIndex ^ compareDistance;
            if (comparePartnerIndex > particleIndex)
            {
                bool ascendingOrder = (particleIndex & sequenceLength) == 0;
                bool swap = (ascendingOrder && sharedKeys[particleIndex].CellHash > sharedKeys[comparePartnerIndex].CellHash) ||
                            (!ascendingOrder && sharedKeys[particleIndex].CellHash < sharedKeys[comparePartnerIndex].CellHash);

                if (swap)
                {
                    ParticleHashData temp = sharedKeys[particleIndex];
                    sharedKeys[particleIndex] = sharedKeys[comparePartnerIndex];
                    sharedKeys[comparePartnerIndex] = temp;
                }
            }

            GroupMemoryBarrierWithGroupSync();
        }
    }

    OutputSortedKeys[particleIndex] = sharedKeys[particleIndex];
}