struct VsOut
{
    float4 color : COLOR;
    float4 pos : SV_Position;
};

cbuffer Cb
{
    matrix mvp;
};

VsOut main( float2 pos : POSITION, float4 color: COLOR )
{
    VsOut vout;
    vout.pos = mul(float4(pos, 0.0f, 1.0f), mvp);
    vout.color = color;
    return vout;
}