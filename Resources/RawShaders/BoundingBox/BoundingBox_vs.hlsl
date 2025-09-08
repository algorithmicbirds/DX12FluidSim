cbuffer CameraBuffer : register(b0)
{
    float4x4 ViewProjection;
};

cbuffer BoundingBoxBuffer : register(b3) 
{
    float2 BBMin;
    float2 BBMax;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;

    float2 corners[5] =
    {
        float2(BBMin.x, BBMin.y),
        float2(BBMax.x, BBMin.y),
        float2(BBMax.x, BBMax.y),
        float2(BBMin.x, BBMax.y),
        float2(BBMin.x, BBMin.y)
    };

    float3 pos = float3(corners[vertexID], 0.0f);

    output.Position = mul(float4(pos, 1.0f), ViewProjection);
    output.Color = float3(1.0f, 0.3f, 0.0f);

    return output;
}
