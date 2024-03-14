cbuffer Colors
{
    float3 colors[6];
};

float4 main(uint tid : SV_PrimitiveID) : SV_TARGET
{
    return float4(colors[tid * 0.5], 1.0f);
}