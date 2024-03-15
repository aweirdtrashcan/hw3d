Texture2D tex : register(t0);

SamplerState samplerState;

float4 main(float2 texCoord : TexCoord) : SV_Target
{
    return tex.Sample(samplerState, texCoord);
}