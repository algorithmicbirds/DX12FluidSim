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
void CSMain(uint tid : SV_GroupThreadID) 
    uint NumBins = 256;

    temp[tid] = (tid < NumBins) ? gHistogram[tid] : 0;
    GroupMemoryBarrierWithGroupSync();

    uint offset = 1;
    for (uint d = NumBins >> 1; d > 0; d >>= 1)
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
        temp[NumBins - 1] = 0;

    for (uint d = 1; d < NumBins; d <<= 1)
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

    if (tid < NumBins)
        gPrefixSum[tid] = temp[tid];
}
