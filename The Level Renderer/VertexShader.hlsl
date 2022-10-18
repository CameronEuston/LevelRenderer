#pragma pack_matrix(row_major)

struct PS_INPUT
{
    float4 posH : SV_POSITION;
    float3 nrmW : NORMAL;
    float3 posW : WORLD;
};

struct VS_INPUT
{
    float3 inputPosition : POSITION;
    float3 inputColor : COLOR;
    float3 inputNormal : NORMAL;
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

ConstantBuffer<SCENE_DATA> cameraAndLights : register(b0, Space0);
ConstantBuffer<MESH_DATA> meshInfo : register(b1, Space0);

// an ultra simple hlsl vertex shader
// TODO: Part 2i
// TODO: Part 2b
// TODO: Part 4f
// TODO: Part 4a
// TODO: Part 1f
// TODO: Part 4b
PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
	
    output.posW = input.inputPosition;
    output.posH = float4(input.inputPosition, 1);
    output.nrmW = input.inputNormal;
	
	// TODO: Part 1h
    output.posH = mul(output.posH, meshInfo.world);
    output.posH = mul(output.posH, cameraAndLights.viewMatrix);
    output.posH = mul(output.posH, cameraAndLights.projectionMatrix);
	
    output.posW = mul(float4(output.posW, 1), meshInfo.world).xyz;
    output.nrmW = mul(float4(output.nrmW, 0), meshInfo.world).xyz;
	
    return output;
	// TODO: Part 4b
}