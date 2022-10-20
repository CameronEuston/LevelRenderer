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

cbuffer VIEW_INFO : register(b2, Space0)
{
    float4x4 viewportMatrix;
    float4 cameraPos;
    //unsigned int padding[]
};

ConstantBuffer<SCENE_DATA> cameraAndLights : register(b0, Space0);
ConstantBuffer<MESH_DATA> meshInfo : register(b1, Space0);

float4 main(PS_INPUT input) : SV_TARGET
{
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
	
    return float4(finalColor); 
}