cbuffer LightCBuf
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
    float3 albedoColor;
    float specularIntensity;
    float3 fresnelR0;
    float specularPower;
    float shininess;
};
 
Texture2D tex;
SamplerState ss;

// attenuation 1 / (CONSTANT + LINEAR * distance + QUADRATIC * distance * distance)

float3 FresnelEffect(float3 R0, float3 normal, float3 lightVec);
float Roughness(float3 halfVec, float3 normal, float shininess);

float4 main(float3 worldPos : Position, float3 wNormal : Normal, float3 wvNormal : wvNormal, float2 texCoord: TexCoord) : SV_Target
{
    float3 lightVec = lightPos - worldPos;
    float distL = length(lightVec);
    lightVec = lightVec / distL;
    
    float3 toEye = normalize(eyePos - worldPos);
    float3 halfVec = normalize(lightVec + toEye);
    
    float3 reflectCoeff = FresnelEffect(fresnelR0, halfVec, lightVec);
    float roughCoeff = Roughness(halfVec, wNormal, shininess);
    
    float att = 1 / (attConst + attLin * distL + attQuad * (distL * distL));
    
    float ndotl = max(dot(lightVec, wNormal), 0);
    float lightStrength = ndotl * diffuseIntensity * att;
    
    float3 diffuse = albedoColor * lightColor;
    float3 ambient = albedoColor * ambientColor;
    
    float3 specular = reflectCoeff * roughCoeff;
    specular = specular / (specular + 1);
    
    specular *= specularIntensity;
    
    float3 specularDiffuse = (diffuse + specular) * lightStrength;
    return float4(specularDiffuse, 1.0f) * tex.Sample(ss, texCoord) + float4(ambient, 1.0f);
}

float3 FresnelEffect(float3 R0, float3 halfVec, float3 lightVec)
{
    float angle = 1.0f - saturate(dot(halfVec, lightVec));
    return R0 + (1.0f - R0) * pow(angle, 5);
}

float Roughness(float3 halfVec, float3 normal, float shininess)
{
    float m = shininess * 256.f;
    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
    
    return roughnessFactor;
}