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

struct PSIn
{
    float3 worldViewPos : Position;
    float3 wNormal : Normal;
    float3 wvNormal : wvNormal;
    float2 texCoord : TexCoord;
    float3 worldPos : PositionW;
    float3 tangent : Tangent;
    float3 biTangent : BiTangent;
};

Texture2D tex : register(t0);
SamplerState ss;
 
// attenuation 1 / (CONSTANT + LINEAR * distance + QUADRATIC * distance * distance)

float4 main(PSIn psin) : SV_Target
{
    float3 lightVec = lightPos - psin.worldViewPos;
    float distL = length(lightVec);
    float3 dirToL = lightVec / distL;
    
    float att = 1 / (attConst + attLin * distL + attQuad * (distL * distL));
    
    const float3 diffuse = diffuseColor * lightColor * diffuseIntensity * att * max(0.0f, dot(dirToL, psin.wvNormal));
	// reflected light vector
    const float3 w = psin.wvNormal * dot(lightVec, psin.wvNormal);
    const float3 r = w * 2.0f - lightVec;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(psin.worldViewPos))), specularPower);
	// final color
    return float4(saturate((diffuse + ambientColor) * tex.Sample(ss, psin.texCoord).rgb + specular), 1.0f);
}