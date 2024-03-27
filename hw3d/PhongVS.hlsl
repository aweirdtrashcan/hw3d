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
    float3 worldPos : PositionW;
    float4 pos : SV_Position;
};
    
VSOut main(float3 pos : Position, float3 normal : Normal)
{
    VSOut vout;
    float4 posf4 = float4(pos, 1.0f);
    vout.pos = mul(posf4, modelViewProj);
    vout.wNormal = normalize(mul(normal, (float3x3) model));
    vout.wvNormal = normalize(mul(normal, (float3x3) modelView));
    vout.worldViewPos = (float3) mul(posf4, modelView);
    vout.worldPos = (float3) mul(posf4, model);
    return vout;
}