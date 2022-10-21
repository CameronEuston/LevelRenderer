#pragma pack_matrix(row_major)
// an ultra simple hlsl pixel shader
// TODO: Part 2b
// TODO: Part 4f
// TODO: Part 4b
struct PS_INPUT
{
    float4 posH : SV_POSITION;
    float3 nrmW : NORMAL;
    float3 posW : WORLD;
};

struct OBJ_ATTRIBUTES
{
    float3 Kd; // diffuse reflectivity
    float d; // dissolve (transparency) 
    float3 Ks; // specular reflectivity
    float Ns; // specular exponent
    float3 Ka; // ambient reflectivity
    float sharpness; // local reflection map sharpness
    float3 Tf; // transmission filter
    float Ni; // optical density (index of refraction)
    float3 Ke; // emissive reflectivity
    unsigned int illum; // illumination model
};

struct SCENE_DATA
{
    float4 sunDirection;
    float4 sunColor;
    float4 sunAmbient;
    float4 camPos;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4 padding[4];
};

struct MESH_DATA
{
    float4x4 world;
    OBJ_ATTRIBUTES material;
    unsigned int padding[28];
};

struct LIGHT_INFO
{
    unsigned int numPointLights, numSpotLights;
    float4x4 pointLights[16], spotLights[16];
};

cbuffer VIEW_INFO : register(b3, Space0)
{
    float4x4 viewportMatrix;
    float4 cameraPos;
    //unsigned int padding[]
};

ConstantBuffer<SCENE_DATA> cameraAndLights : register(b0, Space0);
ConstantBuffer<LIGHT_INFO> lightInfo : register(b2, Space0);
ConstantBuffer<MESH_DATA> meshInfo : register(b1, Space0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 pointLightResult;
    for (int i = 0; i < lightInfo.numPointLights; i++)
    {
        float3 lightDir = normalize(lightInfo.pointLights[i][3].xyz - input.posW);
        float lightRatio = saturate(dot(lightDir, input.nrmW));
        float attenuation = 1.0f - saturate(length(lightInfo.pointLights[i][3].xyz - input.posW) / 5);
        float3 result = attenuation * lightRatio * lightInfo.pointLights[i][1].xyz * meshInfo.material.Kd;
        pointLightResult += result;
    }
    
    float3 spotLightResult;
    for (int i = 0; i < lightInfo.numSpotLights; i++)
    {
        float3 lightDir = normalize(lightInfo.spotLights[i][3].xyz - input.posW);
        float surfaceRatio = saturate(dot(-lightDir, lightInfo.spotLights[i][2].xyz));
        float spotFactor = (surfaceRatio > lightInfo.spotLights[i][1].w) ? 1.0f : 0.0f;
        float lightRatio = saturate(dot(lightDir, input.nrmW));
        float attenuation = 1.0f - saturate((lightInfo.spotLights[i][1].w - surfaceRatio) / (lightInfo.spotLights[i][1].w - lightInfo.spotLights[i][2].w));
        attenuation *= attenuation;
        float3 result = attenuation * spotFactor * lightRatio * lightInfo.spotLights[i][1].xyz * meshInfo.material.Kd;
        
        spotLightResult += result;
    }
    
    input.nrmW = normalize(input.nrmW);
  
	// TODO: Part 3a
    float4 finalColor = float4(0, 0, 0, 0);
    float4 diffuse = float4(meshInfo.material.Kd, meshInfo.material.d);
    float lightRatio = clamp(dot(cameraAndLights.sunDirection.xyz * -1, input.nrmW), 0.0f, 1.0f);
    
    float expo = meshInfo.material.Ns == 0 ? 96 : meshInfo.material.Ns;
    
    float3 viewDirection = normalize(cameraPos.xyz - input.posW);
    float3 halfVector = normalize(cameraAndLights.sunDirection.xyz * -1 + viewDirection);
    float intensity = max(pow(saturate(dot(input.nrmW, halfVector.xyz)), expo), 0);
    float3 reflectedLight = cameraAndLights.sunColor.xyz * meshInfo.material.Ks * intensity;
	
    finalColor = (lightRatio * cameraAndLights.sunColor + cameraAndLights.sunAmbient) * diffuse + float4(reflectedLight, 0);
    finalColor += float4(pointLightResult, 0) + float4(spotLightResult, 0);
    
    return float4(finalColor); 
}