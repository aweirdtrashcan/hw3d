struct VsOut
{
    float4 pos : SV_Position;
};

cbuffer Cb
{
    matrix mvp;
};

VsOut main( float3 pos : POSITION )
{
    VsOut vout;
    vout.pos = mul(float4(pos, 1.0f), mvp);
    return vout;
}