cbuffer CBuf
{
    matrix transformation;
};

struct VSOut
{
    float2 tex : TexCoord;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float2 tex : TexCoord)
{
    VSOut vout;
    vout.pos = mul(float4(pos, 1.0f), transformation);
    vout.tex = tex;
    return vout;
}