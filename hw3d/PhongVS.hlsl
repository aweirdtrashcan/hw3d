struct VSOut
{
    float3 worldViewPos : Position;
    float3 wvNormal: Normal;
    float4 pos : SV_Position;
};

cbuffer CBuf : register(b2)
{
    matrix model;
    matrix modelView;
    matrix modelViewProj;
}

VSOut main(float3 pos : Position, float3 normal : Normal)
{
    VSOut vout;
    const float4 posf4 = float4(pos, 1.0f);
    vout.pos = mul(posf4, modelViewProj);
    vout.worldViewPos = (float3) mul(posf4, modelView);
    vout.wvNormal = mul(normal, (float3x3) modelView);
    return vout;
}