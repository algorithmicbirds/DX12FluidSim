struct Particle
{
    float3 Position;
    float3 Velocity;
};

RWStructuredBuffer<Particle> gParticles : register(u0);

cbuffer TimeBufferCompute : register(b0)
{
    float DeltaTimeCompute;
}

[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    gParticles[DTid.x].Position += float3(0.001f, 0.0f , 0.0f);
}
