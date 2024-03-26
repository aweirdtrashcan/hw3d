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
    float3 albedoColor;
    float specularIntensity;
    float3 fresnelR0;
    float specularPower;
    float shininess;
};

cbuffer CBuf : register(b2)
{
    matrix model;
    matrix modelView;
    matrix modelViewProj;
};

Texture2D tex : register(t0);
Texture2D specularTex : register(t1);
Texture2D normalTex : register(t2);
SamplerState ss;
 
// attenuation 1 / (CONSTANT + LINEAR * distance + QUADRATIC * distance * distance)

float4 main(float3 worldPos : Position, float3 wNormal : Normal, float3 wvNormal : wvNormal, float2 texCoord : TexCoord) : SV_Target
{
    float3 lightVec = lightPos - worldPos;
    float distL = length(lightVec);
    float3 dirToL = lightVec / distL;
    
    float att = 1 / (attConst + attLin * distL + attQuad * (distL * distL));
    
    float3 nNormal = normalTex.Sample(ss, texCoord).xyz;

    nNormal.xz = nNormal.xz * 2.0f - 1.0f;
    nNormal.y = -nNormal.y * 2.0f + 1.0f;
    
    const float3 diffuse = albedoColor * lightColor * diffuseIntensity * att * max(0.0f, dot(dirToL, nNormal));
	// reflected light vector
    const float3 w = nNormal * dot(lightVec, nNormal);
    const float3 r = w * 2.0f - lightVec;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float4 specularSample = specularTex.Sample(ss, texCoord);
    const float3 specularReflectionColor = specularSample.rgb;
    const float specularPower = pow(2.0f, specularSample.a * 13.0f);
    const float3 specular = att * (albedoColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
	// final color
    return float4(saturate((diffuse + ambientColor) * tex.Sample(ss, texCoord).rgb + specular * specularReflectionColor), 1.0f);
}