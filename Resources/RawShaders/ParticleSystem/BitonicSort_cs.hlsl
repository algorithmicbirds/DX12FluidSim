
RWStructuredBuffer<uint> OutputSortedKeys : register(u2);
StructuredBuffer<uint> InputKeys : register(t2);

groupshared uint sharedKeys[256];

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
            if(comparePartnerIndex > particleIndex)
            {
                bool asscendingOrder = (particleIndex & sequenceLength) == 0;
                if ((asscendingOrder && sharedKeys[particleIndex] > sharedKeys[comparePartnerIndex]) 
                    || (!asscendingOrder && sharedKeys[particleIndex] < sharedKeys[comparePartnerIndex]))
                {
                    uint tempKey = sharedKeys[particleIndex];
                    sharedKeys[particleIndex] = sharedKeys[comparePartnerIndex];
                    sharedKeys[comparePartnerIndex] = tempKey;
                }
            }
        }
        
        GroupMemoryBarrierWithGroupSync();
    }   
    
    OutputSortedKeys[particleIndex] = sharedKeys[particleIndex];
}