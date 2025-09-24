struct KeyValue
{
    uint Key;
    uint Value;
};

StructuredBuffer<KeyValue> gInputKeys : register(t2);
RWStructuredBuffer<uint> gHistogram : register(u5);

cbuffer RadixParams : register(b4)
{
    uint ParticleCount;
    uint Shift; 
    uint NumBits;
}

[numthreads(256, 1, 1)]
void CSMain(uint DTid : SV_DispatchThreadID)
{
    uint idx = DTid.x;
    if (idx >= ParticleCount)
        return;
    uint key = gInputKeys[idx].Key;
    uint mask = (1u << NumBits) - 1u;
    uint bin = (key >> Shift) & mask;
    InterlockedAdd(gHistogram[bin], 1);
}
