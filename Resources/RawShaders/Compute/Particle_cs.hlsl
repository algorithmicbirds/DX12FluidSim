struct Particle
{
    float3 Position;
};

RWStructuredBuffer<Particle> gParticles : register(u0);

[numthreads(1, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    gParticles[0].Position += float3(0.1f, 0, 0);
}
