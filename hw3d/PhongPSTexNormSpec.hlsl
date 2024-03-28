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

Texture2D tex : register(t0);
Texture2D specularTex : register(t1);
Texture2D normTex : register(t2);
SamplerState ss;
 
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

// attenuation 1 / (CONSTANT + LINEAR * distance + QUADRATIC * distance * distance)

float4 main(PSIn psin) : SV_Target
{
    float3 lightVec = (float3) mul(float4(lightPos, 1.0f), modelView) - psin.worldViewPos;
    float distL = length(lightVec);
    float3 dirToL = lightVec / distL;
    
    float att = 1 / (attConst + attLin * distL + attQuad * (distL * distL));
    
    float3x3 tanToView = float3x3(
        psin.tangent,
        psin.biTangent,
        psin.wvNormal
    );
    
    float3 n = normTex.Sample(ss, psin.texCoord).xyz;
    n = n * 2.0f - 1.0f;
    n = mul(n, tanToView);
    
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
	// reflected light vector
    const float3 w = n * dot(lightVec, n);
    const float3 r = w * 2.0f - lightVec;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float4 specularSample = specularTex.Sample(ss, psin.texCoord);
    const float3 specularReflectionColor = specularSample.rgb;
    float specularPower = specularPowerConst;
    
    if (hasGloss)
    {
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    }
    
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(psin.worldViewPos))), specularPower);
	// final color
    return float4(saturate((diffuse + ambientColor) * tex.Sample(ss, psin.texCoord).rgb + specular * specularReflectionColor), 1.0f);
}