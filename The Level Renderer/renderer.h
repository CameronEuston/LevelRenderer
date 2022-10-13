// minimalistic code to draw a single triangle, this is not part of the API.
// required for compiling shaders on the fly, consider pre-compiling instead
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include "d3dx12.h" // official helper file provided by microsoft
#include "Model.h"

// Simple Vertex Shader
const char* vertexShaderSource = R"()";
// Simple Pixel Shader
const char* pixelShaderSource = R"()";

std::vector<std::string> SplitString(std::string s, char delimiter)
{
	std::vector<std::string> result;
	std::string currentString;

	for (int i = 0; i < s.length() - 1; i++)
	{
		if (s.at(i) == delimiter)
		{
			result.push_back(currentString);
			currentString = "";
		}
		else
		{
			currentString.push_back(s.at(i));
		}
	}
	result.push_back(currentString);

	return result;
}

// Creation, Rendering & Cleanup
class Renderer
{
	std::string ShaderAsString(const char* shaderFilePath)
	{
		std::string output;
		unsigned int stringLength = 0;
		GW::SYSTEM::GFile file; file.Create();
		file.GetFileSize(shaderFilePath, stringLength);
		if (stringLength && +file.OpenBinaryRead(shaderFilePath))
		{
			output.resize(stringLength);
			file.Read(&output[0], stringLength);
		}
		else
			std::cout << "ERROR: Shader Source File \"" << shaderFilePath << "\" Not Found!" << std::endl;
		return output;
	}

	// proxy handles
	GW::SYSTEM::GWindow win;
	GW::GRAPHICS::GDirectX12Surface d3d;

	GW::INPUT::GInput input;
	GW::INPUT::GController controller;

	DirectX::XMMATRIX viewMatrix;

	DirectX::XMMATRIX projectionMatrix;

	float verticalFOV;
	float nearPlane;
	float farPlane;
	float aspectRatio;

	SCENE_DATA sceneData;

	std::chrono::steady_clock::time_point pastTime;
	std::chrono::steady_clock::time_point nowTime;

	// what we need at a minimum to draw a triangle
	Microsoft::WRL::ComPtr<ID3D12RootSignature>	rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>	pipeline;

	H2B::Parser parser;
public:
	std::vector<Model> models;

	Renderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GDirectX12Surface _d3d)
	{
		win = _win;
		d3d = _d3d;
		ID3D12Device* creator;
		d3d.GetDevice((void**)&creator);

		input.Create(win);
		controller.Create();


		viewMatrix = DirectX::XMMatrixIdentity();
		viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, DirectX::XMMatrixTranslation(0, 0, 0));
		viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, DirectX::XMMatrixRotationX(0));
		viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, DirectX::XMMatrixRotationY(0));
		DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(viewMatrix);
		viewMatrix = DirectX::XMMatrixInverse(&determinant, viewMatrix);
		
		verticalFOV = 65 * pi / 180;
		nearPlane = .1f;
		farPlane = 100;
		d3d.GetAspectRatio(aspectRatio);

		float yScale = cosf(verticalFOV * .5f) / sinf(verticalFOV * .5f);
		float xScale = yScale * aspectRatio;

		projectionMatrix.r[0] = {xScale, 0, 0, 0};
		projectionMatrix.r[1] = {0, yScale, 0, 0 };
		projectionMatrix.r[2] = {0, 0, farPlane / (farPlane - nearPlane), 1};
		projectionMatrix.r[3] = {0, 0, -(farPlane * nearPlane) / (farPlane - nearPlane), 0};

		sceneData.sunDirection = { -1, -1, 2, 0 };
		sceneData.sunDirection = DirectX::XMVector4Normalize(sceneData.sunDirection);
		sceneData.sunColor = { .9f, .9f, 1, 1 };
		sceneData.viewMatrix = viewMatrix;
		sceneData.projectionMatrix = projectionMatrix;
		sceneData.sunAmbient = { .25f, .25f, .35f };

		// Create Vertex Shader
		UINT compilerFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
		compilerFlags |= D3DCOMPILE_DEBUG;
#endif
		std::string pixelShader = ShaderAsString("../PixelShader.hlsl");
		std::string vertexShader = ShaderAsString("../VertexShader.hlsl");

		Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, errors;
		if (FAILED(D3DCompile(vertexShader.c_str(), strlen(vertexShader.c_str()),
			nullptr, nullptr, nullptr, "main", "vs_5_1", compilerFlags, 0,
			vsBlob.GetAddressOf(), errors.GetAddressOf())))
		{
			std::cout << (char*)errors->GetBufferPointer() << std::endl;
			abort();
		}
		// Create Pixel Shader
		Microsoft::WRL::ComPtr<ID3DBlob> psBlob; errors.Reset();
		if (FAILED(D3DCompile(pixelShader.c_str(), strlen(pixelShader.c_str()),
			nullptr, nullptr, nullptr, "main", "ps_5_1", compilerFlags, 0,
			psBlob.GetAddressOf(), errors.GetAddressOf())))
		{
			std::cout << (char*)errors->GetBufferPointer() << std::endl;
			abort();
		}
		// Create Input Layout
		D3D12_INPUT_ELEMENT_DESC format[] = 
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		CD3DX12_ROOT_PARAMETER rootParams[2];

		rootParams[0].InitAsConstantBufferView(0);
		rootParams[1].InitAsConstantBufferView(1);

		CD3DX12_ROOT_SIGNATURE_DESC rootDesc;
		rootDesc.Init(2, rootParams, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		CD3DX12_ROOT_PARAMETER rootParam;
		rootParam.InitAsConstants(48, 0);
		// create root signature
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(1, &rootParam, 0, nullptr, 
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> signature;
		D3D12SerializeRootSignature(&rootDesc, 
			D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errors);
		creator->CreateRootSignature(0, signature->GetBufferPointer(), 
			signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

		// create pipeline state
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psDesc;
		ZeroMemory(&psDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		psDesc.InputLayout = { format, ARRAYSIZE(format) };
		psDesc.pRootSignature = rootSignature.Get();
		psDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
		psDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
		psDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psDesc.SampleMask = UINT_MAX;
		psDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psDesc.NumRenderTargets = 1;
		psDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psDesc.SampleDesc.Count = 1;
		creator->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(&pipeline));
		// free temporary handle
		creator->Release();

		pastTime = std::chrono::high_resolution_clock::now();

		ParseData();
	}
	void Render()
	{
		// grab the context & render target
		ID3D12GraphicsCommandList* cmd;
		D3D12_CPU_DESCRIPTOR_HANDLE rtv;
		D3D12_CPU_DESCRIPTOR_HANDLE dsv;
		d3d.GetCommandList((void**)&cmd);
		d3d.GetCurrentRenderTargetView((void**)&rtv);
		d3d.GetDepthStencilView((void**)&dsv);
		// setup the pipeline
		cmd->SetGraphicsRootSignature(rootSignature.Get());
		cmd->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
		cmd->SetPipelineState(pipeline.Get());
		cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// now we can draw
		for (int i = 0; i < models.size(); i++)
		{
			UINT8* constantBufferMemoryLocation;
			models[i].constantBuffer->Map(0, &CD3DX12_RANGE(0, 0),
				reinterpret_cast<void**>(&constantBufferMemoryLocation));
			memcpy(constantBufferMemoryLocation, &sceneData, sizeof(sceneData));
			models[i].constantBuffer->Unmap(0, nullptr);

			cmd->IASetVertexBuffers(0, 1, &(models[i].vertexView));
			cmd->IASetIndexBuffer(&(models[i].indexView));

			cmd->SetDescriptorHeaps(1, models[i].descriptorHeap);

			cmd->SetGraphicsRootConstantBufferView(0, models[i].constantBuffer.Get()->GetGPUVirtualAddress());
			for (int j = 0; j < models[i].meshCount; j++)
			{
				cmd->SetGraphicsRootConstantBufferView(1, models[i].constantBuffer.Get()->GetGPUVirtualAddress() + sizeof(sceneData) + (sizeof(MESH_DATA) * j));
				cmd->DrawIndexedInstanced(models[i].meshes[j].drawInfo.indexCount, 1, models[i].meshes[j].drawInfo.indexOffset, 0, 0);
			}
		}

		// release temp handles
		cmd->Release();
	}
	void UpdateCamera()
	{
		nowTime = std::chrono::high_resolution_clock::now();

		std::chrono::duration<float> duration = nowTime - pastTime;

		//inversing into world space to transform the cameras position
		DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(viewMatrix);
		viewMatrix = DirectX::XMMatrixInverse(&determinant, viewMatrix);

		float yChange = 0;
		float xChange = 0;
		float zChange = 0;
		const float Camera_Speed = .3f;

		float space = 0;
		float leftShift = 0;
		float rightTrigger = 0;
		float leftTrigger = 0;
		float w = 0;
		float s = 0;
		float leftStickY = 0;
		float d = 0;
		float a = 0;
		float leftStickX = 0;
		float mouseX = 0;
		float mouseY = 0;
		float rightStickY = 0;
		float rightStickX = 0;
		float aButton = 0;
		float stickButton = 0;
		unsigned int screenHeight = 0;
		unsigned int screenWidth = 0;
		input.GetMouseDelta(mouseX, mouseY);
		win.GetHeight(screenHeight);
		win.GetWidth(screenWidth);
		float thumbSpeed = pi * duration.count();
		input.GetState(G_KEY_SPACE, space);
		input.GetState(G_KEY_LEFTSHIFT, leftShift);
		controller.GetState(0, G_RIGHT_TRIGGER_AXIS, rightTrigger);
		controller.GetState(0, G_LEFT_TRIGGER_AXIS, leftTrigger);
		controller.GetState(0, G_SOUTH_BTN, aButton);
		controller.GetState(0, G_RIGHT_THUMB_BTN, stickButton);
		input.GetState(G_KEY_W, w);
		input.GetState(G_KEY_S, s);
		controller.GetState(0, G_LY_AXIS, leftStickY);
		input.GetState(G_KEY_A, a);
		input.GetState(G_KEY_D, d);
		controller.GetState(0, G_LX_AXIS, leftStickX);
		controller.GetState(0, G_RY_AXIS, rightStickY);
		controller.GetState(0, G_RX_AXIS, rightStickX);

		xChange = (d - a + leftStickX) * Camera_Speed * duration.count();

		float upY = max(aButton, rightTrigger);
		float downY = max(stickButton, leftTrigger);
		yChange = (space - leftShift + upY - downY) * Camera_Speed * duration.count();
		zChange = (w - s + leftStickY) * Camera_Speed * duration.count();
		float totalPitch = verticalFOV * mouseY / screenHeight + rightStickY * -thumbSpeed;
		float totalYaw = verticalFOV * mouseX / screenWidth + rightStickX * thumbSpeed;

		viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, DirectX::XMMatrixTranslation(0, yChange, 0));
		
	
		//inversing back into view space
		viewMatrix = DirectX::XMMatrixInverse(&determinant, viewMatrix);

		viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, DirectX::XMMatrixTranslation(-xChange, 0, -zChange));
		viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, DirectX::XMMatrixRotationX(-totalPitch));
		
		viewMatrix = DirectX::XMMatrixInverse(&determinant, viewMatrix);
		DirectX::XMVECTOR position = viewMatrix.r[3];
		viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, DirectX::XMMatrixRotationY(totalYaw));
		viewMatrix.r[3] = position;
		viewMatrix = DirectX::XMMatrixInverse(&determinant, viewMatrix);

		sceneData.viewMatrix = viewMatrix;

		pastTime = nowTime;
	}
	~Renderer()
	{
		// ComPtr will auto release so nothing to do here 
	}

	void ParseData()
	{
		std::ifstream inputStream;
		inputStream.open("../GameLevel.txt");

		if (inputStream.is_open())
		{
			std::string lineInfo = "";
			std::string objName = "";

			while (true)
			{
				std::getline(inputStream, lineInfo);

				if (lineInfo.compare("MESH") == 0)
				{
					DirectX::XMMATRIX newMesh;

					std::getline(inputStream, objName);
					int periodPos = objName.find_last_of('.');
					if (periodPos != -1)
						objName.erase(periodPos);
					objName.append(".h2b");

					DirectX::XMVECTOR vector;
					std::string stringVector;
					std::string extractedNumber;
					std::vector<std::string> coordinates;

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newMesh.r[0] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newMesh.r[1] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newMesh.r[2] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newMesh.r[3] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					objName = "../Models/" + objName;

					if (parser.Parse(objName.c_str()) && objName == "../Models/Triceratops.h2b")
					{

						Model model;
						
						model.meshData.world = newMesh;

						//index buffer
						model.indexCount = parser.indexCount;
						model.indices = parser.indices;
						ID3D12Device* creator;
						d3d.GetDevice((void**)&creator);

						creator->CreateCommittedResource( // using UPLOAD heap for simplicity
							&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // DEFAULT recommend  
							D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(unsigned int) * model.indices.size()),
							D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&(model.indexBuffer)));
						// Transfer triangle data to the index buffer.
						UINT8* memoryLocation;
						model.indexBuffer->Map(0, &CD3DX12_RANGE(0, 0),
							reinterpret_cast<void**>(&memoryLocation));
						memcpy(memoryLocation, model.indices.data(), sizeof(unsigned int) * model.indices.size());
						model.indexBuffer->Unmap(0, nullptr);
						// Create a index View to send to a Draw() call.
						model.indexView.BufferLocation = model.indexBuffer->GetGPUVirtualAddress();
						model.indexView.SizeInBytes = sizeof(unsigned int) * model.indices.size(); // TODO: Part 1d
						model.indexView.Format = DXGI_FORMAT_R32_UINT;

						//create the vertex buffer (green check)
						model.vertexCount = parser.vertexCount;
						model.vertices = parser.vertices;
						creator->CreateCommittedResource( // using UPLOAD heap for simplicity
							&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // DEFAULT recommend  
							D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(H2B::VERTEX) * model.vertices.size()),
							D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&(model.vertexBuffer)));
						// Transfer triangle data to the vertex buffer.
						UINT8* transferMemoryLocation;
						model.vertexBuffer->Map(0, &CD3DX12_RANGE(0, 0),
							reinterpret_cast<void**>(&transferMemoryLocation));
						memcpy(transferMemoryLocation, model.vertices.data(), sizeof(H2B::VERTEX) * model.vertices.size());
						model.vertexBuffer->Unmap(0, nullptr);
						// Create a vertex View to send to a Draw() call.
						model.vertexView.BufferLocation = model.vertexBuffer->GetGPUVirtualAddress();
						model.vertexView.StrideInBytes = sizeof(H2B::VERTEX); // TODO: Part 1e
						model.vertexView.SizeInBytes = sizeof(H2B::VERTEX) * model.vertices.size(); // TODO: Part 1d

						model.materialCount = parser.materialCount;
						model.materials = parser.materials;

						model.meshCount = parser.meshCount;
						model.meshes = parser.meshes;

						//constant buffer
						IDXGISwapChain4* swapChain;
						DXGI_SWAP_CHAIN_DESC swapChainDesc;
						d3d.GetSwapchain4((void**)&swapChain);
						swapChain->GetDesc(&swapChainDesc);
						unsigned int bufferByteSize = (sizeof(sceneData) + model.meshCount * sizeof(MESH_DATA)) * swapChainDesc.BufferCount;
						creator->CreateCommittedResource( // using UPLOAD heap for simplicity
							&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // DEFAULT recommend  
							D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferByteSize),
							D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&(model.constantBuffer)));
						// Transfer triangle data to the index buffer.
						UINT8* constantBufferMemoryLocation;
						model.constantBuffer->Map(0, &CD3DX12_RANGE(0, 0),
							reinterpret_cast<void**>(&constantBufferMemoryLocation));
						memcpy(constantBufferMemoryLocation, &sceneData, sizeof(sceneData));
						constantBufferMemoryLocation += sizeof(sceneData);
						for (int i = 0; i < model.meshCount; i++)
						{
							model.meshData.material = model.materials[i].attrib;
							memcpy(constantBufferMemoryLocation, &model.meshData, sizeof(MESH_DATA));
							constantBufferMemoryLocation += sizeof(MESH_DATA);
						}

						model.constantBuffer->Unmap(0, nullptr);

						D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
						descriptorHeapDesc.NumDescriptors = swapChainDesc.BufferCount;
						descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
						descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

						creator->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&(model.descriptorHeap[0])));

						D3D12_CONSTANT_BUFFER_VIEW_DESC bufferDesc;
						bufferDesc.BufferLocation = model.constantBuffer.Get()->GetGPUVirtualAddress();
						bufferDesc.SizeInBytes = bufferByteSize;

						D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = model.descriptorHeap[0]->GetCPUDescriptorHandleForHeapStart();

						creator->CreateConstantBufferView(&bufferDesc, descriptorHandle);

						models.push_back(model);
					}

					std::cout << objName << std::endl;
					for (int i = 0; i < 4; i++)
					{
						std::cout << DirectX::XMVectorGetX(newMesh.r[i]) << ' ' << DirectX::XMVectorGetY(newMesh.r[i]) << ' ' << DirectX::XMVectorGetZ(newMesh.r[i]) << ' ' << DirectX::XMVectorGetW(newMesh.r[i]) << ' ' << std::endl;
					}
				}

				std::cout << std::endl;

				if (inputStream.eof())
				{
					break;
				}
			}

			inputStream.close();
		}
	}
};
