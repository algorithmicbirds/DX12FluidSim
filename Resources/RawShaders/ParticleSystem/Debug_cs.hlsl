
struct ParticleCellHash
{
    uint ParticleHash;
    uint ParticleIndex;
};


RWStructuredBuffer<ParticleCellHash> SortedKeys : register(u1);
RWStructuredBuffer<ParticleCellHash> DebugData : register(u5);

[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint id = DTid.x;
    DebugData[id] = SortedKeys[id];
}