StructuredBuffer<uint> gHistogram : register(t6);
RWStructuredBuffer<uint> gPrefixSum : register(u6);

cbuffer RadixParams : register(b4)
{
    uint ParticleCount;
    uint Shift;
    uint NumBits; 
}

groupshared uint temp[256];

[numthreads(256, 1, 1)]
void CSMain(uint tid : SV_DispatchThreadID)
{
    uint index = tid;
    uint NumBins = 256; 

    temp[tid] = (index < NumBins) ? gHistogram[index] : 0;
    GroupMemoryBarrierWithGroupSync();

    uint offset = 1;
    for (uint d = 256 >> 1; d > 0; d >>= 1)
    {
        GroupMemoryBarrierWithGroupSync();
        if (tid < d)
        {
            uint ai = offset * (2 * tid + 1) - 1;
            uint bi = offset * (2 * tid + 2) - 1;
            temp[bi] += temp[ai];
        }
        offset <<= 1;
    }


    if (tid == 0)
        temp[255] = 0;

    for (uint d = 1; d < 256; d <<= 1)
    {
        offset >>= 1;
        GroupMemoryBarrierWithGroupSync();
        if (tid < d)
        {
            uint ai = offset * (2 * tid + 1) - 1;
            uint bi = offset * (2 * tid + 2) - 1;
            uint t = temp[ai];
            temp[ai] = temp[bi];
            temp[bi] += t;
        }
    }
    GroupMemoryBarrierWithGroupSync();

    if (index < NumBins)
        gPrefixSum[index] = temp[tid];
}
