#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include <shobjidl.h>

float pi = 3.14159265359;

struct Vertex
{
	float x, y, z, w;
};

struct SCENE_DATA
{
	DirectX::XMVECTOR sunDirection, sunColor, sunAmbient, camPos;
	DirectX::XMMATRIX viewMatrix, projectionMatrix;
	DirectX::XMVECTOR padding[4];
};

struct VIEW_INFO
{
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMVECTOR cameraPos;
};

struct LIGHT_INFO
{
	unsigned int numPointLights, numSpotLights;
	DirectX::XMMATRIX pointLights[16];
	DirectX::XMMATRIX spotLights[16];
};
