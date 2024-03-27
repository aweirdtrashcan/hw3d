cbuffer CBuf : register(b2)
{
    matrix model;
    matrix modelView;
    matrix modelViewProj;
};

struct VSOut
{
    float3 worldViewPos : Position;
    float3 wNormal : Normal;
    float3 wvNormal : wvNormal;
    float2 texCoord : TexCoord;
    float3 worldPos : PositionW;
    float3 tangent : Tangent;
    float3 biTangent : BiTangent;
    float4 pos : SV_Position;
};
    
VSOut main(float3 pos : Position, float3 normal : Normal, float2 texCoord : TexCoord, float3 tangent : Tangent, float3 biTangent : BiTangent)
{
    VSOut vout;
    float4 posf4 = float4(pos, 1.0f);
    vout.pos = mul(posf4, modelViewProj);
    vout.wNormal = normalize(mul(normal, (float3x3) model));
    vout.wvNormal = normalize(mul(normal, (float3x3) modelView));
    vout.worldViewPos = (float3) mul(posf4, modelView);
    vout.worldPos = (float3) mul(posf4, model);
    vout.tangent = normalize(mul(tangent, (float3x3) modelView));
    vout.biTangent = normalize(mul(biTangent, (float3x3) modelView));
    vout.texCoord = texCoord;
    return vout;
}