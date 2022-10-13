#pragma once
#include "h2bParser.h"

struct MESH_DATA
{
	DirectX::XMMATRIX world;
	H2B::ATTRIBUTES material;
	unsigned int padding[28];
};

class Model
{
public:
	D3D12_VERTEX_BUFFER_VIEW					vertexView;
	Microsoft::WRL::ComPtr<ID3D12Resource>		vertexBuffer;
	D3D12_INDEX_BUFFER_VIEW						indexView;
	Microsoft::WRL::ComPtr<ID3D12Resource>		indexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>		constantBuffer;
	ID3D12DescriptorHeap* descriptorHeap[1];
	Microsoft::WRL::ComPtr<ID3D12RootSignature>	rootSignature;

	MESH_DATA meshData;
	unsigned vertexCount;
	unsigned indexCount;
	unsigned materialCount;
	unsigned meshCount;
	std::vector<H2B::VERTEX> vertices;
	std::vector<unsigned> indices;
	std::vector<H2B::MATERIAL> materials;
	std::vector<H2B::MESH> meshes;
};