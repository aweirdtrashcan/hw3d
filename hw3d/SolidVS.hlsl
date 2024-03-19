cbuffer CBuf
{
    matrix model;
    matrix view;
    matrix modelViewProj;
};

float4 main(float3 pos : Position) : SV_Position
{
    return mul(float4(pos, 1.0f), modelViewProj);
}