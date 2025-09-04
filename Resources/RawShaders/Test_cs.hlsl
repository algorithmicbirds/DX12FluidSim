RWTexture2D<float4> OutputTex : register(u0);

[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    float2 uv = float2(DTid.xy) / float2(512, 512);
    OutputTex[DTid.xy] = float4(uv, 1.0f, 1.0f);
}
