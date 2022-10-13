#pragma once
#include <DirectXMath.h>

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

struct SHADER_VARS
{
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
};
