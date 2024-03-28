cbuffer LightCBuf : register(b0)
{
    float3 lightPos;
    float diffuseIntensity;
    float3 ambientColor;
    float attConst;
    float3 lightColor;
    float attLin;
    float3 eyePos;
    float attQuad;
};

cbuffer MaterialCBuf : register(b1)
{
    float3 diffuseColor;
    float specularIntensity;
    float specularPower;
    float shininess;
    bool hasGloss;
    float specularPowerConst;
};

cbuffer CBuf : register(b2)
{
    matrix model;
    matrix modelView;
    matrix modelViewProj;
};

struct PSIn
{
    float3 worldViewPos : Position;
    float3 wvNormal : Normal;
};

float4 main(PSIn psin) : SV_TARGET
{
    const float3 vToL = (float3) mul(float4(lightPos, 1.0f), modelView) - psin.worldViewPos;
    const float distL = length(vToL);
    const float3 dirToL = vToL / distL;
    
    const float att = 1.0f / (attConst * attLin + distL * attQuad * (distL * distL));
    
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, psin.wvNormal));
    
    const float3 w = psin.wvNormal * (vToL * psin.wvNormal);
    const float3 r = w * 2.0f - vToL;
    
    float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(psin.worldViewPos))), specularPower);
    
    return float4(saturate(diffuse + ambientColor + specular), 1.0f);
    
}