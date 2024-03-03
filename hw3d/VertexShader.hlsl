struct VsOut
{
    float4 color : COLOR;
    float4 pos : SV_Position;
};

VsOut main( float2 pos : POSITION, float4 color: COLOR )
{
    VsOut vout;
    vout.pos = float4(pos, 0.0f, 1.0f);
    vout.color = color;
    return vout;
}