struct KeyValue
{
    uint Key;
    uint Value;
};

StructuredBuffer<KeyValue> gInputKeys : register(t2); 
StructuredBuffer<uint> gPrefixSum : register(t7); 
RWStructuredBuffer<KeyValue> gOutputKeys : register(u7); 
RWStructuredBuffer<uint> gBinOffsets : register(u8); 

cbuffer RadixParams : register(b4)
{
    uint ParticleCount;
    uint Shift; 
    uint NumBits; 
};

[numthreads(256, 1, 1)]
void CSMain(uint tid : SV_DispatchThreadID)
{
    if (tid >= ParticleCount)
        return;

    KeyValue kv = gInputKeys[tid];

    uint binMask = (1u << NumBits) - 1u;
    uint bin = (kv.Key >> Shift) & binMask;

    uint localOffset;
    InterlockedAdd(gBinOffsets[bin], 1, localOffset);

    uint destIndex = gPrefixSum[bin] + localOffset;

    gOutputKeys[destIndex] = kv;
}
