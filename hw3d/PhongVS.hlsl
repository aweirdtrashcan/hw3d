cbuffer CBuf
{
    matrix model;
    matrix modelViewProj;
};

struct VSOut
{
    float3 worldPos : Position;
    float3 normal : Normal;
    float4 pos : SV_Position;
};
    
VSOut main( float3 pos : Position, float3 normal : Normal )
{
    VSOut vout;
    float4 posf4 = float4(pos, 1.0f);
    vout.pos = mul(posf4, modelViewProj);
    vout.normal = mul(normal, (float3x3)model);
    vout.worldPos = (float3) mul(posf4, model);
    return vout;
}