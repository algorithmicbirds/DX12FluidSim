
struct Particle
{
    float3 Position;
};

RWStructuredBuffer<Particle> gParticle : register(u0);
RWStructuredBuffer<uint> gMortonCode : register(u1);

// ExpandCoordinateBits function adapted from Sean Eron Anderson, "Bit Twiddling Hacks" (public domain)
// https://graphics.stanford.edu/~seander/bithacks.html#InterleaveBMN
uint ExpandCoordinateBits(uint cellCoordinate)
{
    cellCoordinate = (cellCoordinate | (cellCoordinate << 8)) & 0x00FF00FF;
    cellCoordinate = (cellCoordinate | (cellCoordinate << 4)) & 0x0F0F0F0F;
    cellCoordinate = (cellCoordinate | (cellCoordinate << 2)) & 0x33333333;
    cellCoordinate = (cellCoordinate | (cellCoordinate << 1)) & 0x55555555;
    return cellCoordinate;
}

uint EncodeMorton(uint cellX, uint cellY)
{
    uint expandCellx = ExpandCoordinateBits(cellX);
    uint expandCelly = ExpandCoordinateBits(cellY);
    return (expandCellx << 1) | expandCelly;
}

[numthreads(256, 1 , 1)]
void CSMain(uint DTid : SV_DispatchThreadID)
{
    Particle particle = gParticle[DTid.x];
    float cellSize = 0.05f;
    uint particleCellX = (uint) floor(particle.Position.x / cellSize);
    uint particleCellY = (uint) floor(particle.Position.y / cellSize);
    
    gMortonCode[DTid] = EncodeMorton(particleCellX, particleCellY);
}