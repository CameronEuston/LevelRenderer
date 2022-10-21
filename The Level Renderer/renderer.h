// minimalistic code to draw a single triangle, this is not part of the API.
// required for compiling shaders on the fly, consider pre-compiling instead
#include <DDSTextureLoader.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include "d3dx12.h" // official helper file provided by microsoft
#include "Model.h"

// Simple Vertex Shader
const char* vertexShaderSource = R"()";
// Simple Pixel Shader
const char* pixelShaderSource = R"()";

const char* texturePixelShaderSource = R"()";
const char* textureVertexShaderSource = R"()";

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

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureUpload;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>	texturePipeline;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>	textureRootSignature;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>	cbvHeap;
	UINT											cbvDescriptorSize;
	Microsoft::WRL::ComPtr<ID3D12Resource>			cbvResource;
	GW::MATH::GMATRIXF* cbvResourceData;

	H2B::Parser parser;
	Model skybox;
	std::string levelName;

	D3D12_VIEWPORT viewports[2];

	//int lightRootConstantSize;

public:
	std::vector<Model> models;
	std::vector<DirectX::XMMATRIX> views;
	std::vector<DirectX::XMMATRIX> pointLights;
	std::vector<DirectX::XMMATRIX> spotLights;

	Renderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GDirectX12Surface _d3d)
	{
		viewports[0].MinDepth = .2f;
		viewports[0].MaxDepth = 1;
		viewports[0].TopLeftX = 0;
		viewports[0].TopLeftY = 0;

		viewports[1].MinDepth = 0;
		viewports[1].MaxDepth = .2f;
		viewports[1].TopLeftX = 0;
		viewports[1].TopLeftY = 0;

		levelName = "../GameLevel.txt";
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

		projectionMatrix.r[0] = { xScale, 0, 0, 0 };
		projectionMatrix.r[1] = { 0, yScale, 0, 0 };
		projectionMatrix.r[2] = { 0, 0, farPlane / (farPlane - nearPlane), 1 };
		projectionMatrix.r[3] = { 0, 0, -(farPlane * nearPlane) / (farPlane - nearPlane), 0 };

		sceneData.sunDirection = { -1, -1, 2, 0 };
		sceneData.sunDirection = DirectX::XMVector4Normalize(sceneData.sunDirection);
		sceneData.sunColor = { .9f, .9f, 1.0f, 1 };
		sceneData.viewMatrix = viewMatrix;
		sceneData.projectionMatrix = projectionMatrix;
		sceneData.sunAmbient = { .25f, .25f, .35f, 1 };
		sceneData.camPos = { 0, 0, 0, 0 };

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
		
		//normal root param stuff
		CD3DX12_ROOT_PARAMETER rootParams[3];

		rootParams[0].InitAsConstantBufferView(0);
		rootParams[1].InitAsConstantBufferView(1);
		rootParams[2].InitAsConstants(20, 2);

		CD3DX12_ROOT_SIGNATURE_DESC rootDesc;
		rootDesc.Init(3, rootParams, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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
		//CreateSkyBox(skybox);
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

		unsigned int width = 0;
		unsigned int height = 0;

		win.GetWidth(width);
		viewports[0].Width = (float)width;
		win.GetHeight(height);
		viewports[0].Height = (float)height;

		viewports[1].Width = width / 4.0f;
		viewports[1].Height = height / 4.0f;

		cmd->RSSetViewports(1, &viewports[0]);

		VIEW_INFO playerCam;

		playerCam.viewMatrix = viewMatrix;

		sceneData.viewMatrix = viewMatrix;

		DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(viewMatrix);
		viewMatrix = DirectX::XMMatrixInverse(&determinant, viewMatrix);
		sceneData.camPos = viewMatrix.r[3];
		playerCam.cameraPos = viewMatrix.r[3];
		viewMatrix = DirectX::XMMatrixInverse(&determinant, viewMatrix);

		cmd->SetGraphicsRoot32BitConstants(2, 20, &playerCam, 0);

		// now we can draw
		for (int i = 0; i < models.size(); i++)
		{

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

		if (views.size() > 0)
		{
			cmd->RSSetViewports(1, &viewports[1]);

			VIEW_INFO presetCam;
			presetCam.viewMatrix = views[0];

			sceneData.viewMatrix = views[0];

			DirectX::XMVECTOR determinantPreset = DirectX::XMMatrixDeterminant(views[0]);
			views[0] = DirectX::XMMatrixInverse(&determinantPreset, views[0]);
			sceneData.camPos = views[0].r[3];
			presetCam.cameraPos = views[0].r[3];
			views[0] = DirectX::XMMatrixInverse(&determinantPreset, views[0]);

			cmd->SetGraphicsRoot32BitConstants(2, 20, &presetCam, 0);

			// now we can draw
			for (int i = 0; i < models.size(); i++)
			{
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
		}

		sceneData.viewMatrix = viewMatrix;


		viewMatrix = DirectX::XMMatrixInverse(&determinant, viewMatrix);

		sceneData.camPos = viewMatrix.r[3];

		viewMatrix = DirectX::XMMatrixInverse(&determinant, viewMatrix);

		//UINT8* constantBufferMemoryLocation;
		//skybox.constantBuffer->Map(0, &CD3DX12_RANGE(0, 0),
		//	reinterpret_cast<void**>(&constantBufferMemoryLocation));
		//memcpy(constantBufferMemoryLocation + sizeof(sceneData), &(skybox.meshData), sizeof(MESH_DATA));
		//skybox.constantBuffer->Unmap(0, nullptr);

		////render the skybox
		//cmd->IASetVertexBuffers(0, 1, &(skybox.vertexView));
		//cmd->IASetIndexBuffer(&(skybox.indexView));

		//cmd->SetDescriptorHeaps(1, skybox.descriptorHeap);

		//cmd->SetGraphicsRootConstantBufferView(0, skybox.constantBuffer.Get()->GetGPUVirtualAddress());

		//cmd->SetGraphicsRootConstantBufferView(1, skybox.constantBuffer.Get()->GetGPUVirtualAddress() + sizeof(sceneData));
		//cmd->DrawIndexedInstanced(skybox.meshes[0].drawInfo.indexCount, 1, skybox.meshes[0].drawInfo.indexOffset, 0, 0);

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
		const float Camera_Speed = 3.0f;

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

		if (input.GetMouseDelta(mouseX, mouseY) == GW::GReturn::REDUNDANT)
		{
			mouseX = 0;
			mouseY = 0;
		}
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

		DirectX::XMVECTOR worldTranslation = { 0, yChange, 0 };
		viewMatrix.r[3] = DirectX::XMVectorAdd(viewMatrix.r[3], worldTranslation);


		//inversing back into view space
		viewMatrix = DirectX::XMMatrixInverse(&determinant, viewMatrix);

		DirectX::XMVECTOR localTranslation = { -xChange, 0, -zChange };
		viewMatrix.r[3] = DirectX::XMVectorAdd(viewMatrix.r[3], localTranslation);

		viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, DirectX::XMMatrixRotationX(-totalPitch));

		viewMatrix = DirectX::XMMatrixInverse(&determinant, viewMatrix);
		DirectX::XMVECTOR position = viewMatrix.r[3];
		viewMatrix = DirectX::XMMatrixMultiply(viewMatrix, DirectX::XMMatrixRotationY(totalYaw));
		viewMatrix.r[3] = position;
		sceneData.camPos = position;

		skybox.meshData.world = DirectX::XMMatrixIdentity();
		skybox.meshData.world *= 95;
		skybox.meshData.world.r[3] = position;

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
		inputStream.open(levelName);

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

					if (parser.Parse(objName.c_str()))
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

						creator->Release();
					}

					std::cout << objName << std::endl;
					for (int i = 0; i < 4; i++)
					{
						std::cout << DirectX::XMVectorGetX(newMesh.r[i]) << ' ' << DirectX::XMVectorGetY(newMesh.r[i]) << ' ' << DirectX::XMVectorGetZ(newMesh.r[i]) << ' ' << DirectX::XMVectorGetW(newMesh.r[i]) << ' ' << std::endl;
					}
				}
				else if (lineInfo.compare("CAMERA") == 0)
				{
					DirectX::XMMATRIX newView;

					std::getline(inputStream, objName);
					int periodPos = objName.find_last_of('.');
					if (periodPos != -1)
						objName.erase(periodPos);

					DirectX::XMVECTOR vector;
					std::string stringVector;
					std::string extractedNumber;
					std::vector<std::string> coordinates;

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newView.r[0] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newView.r[1] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newView.r[2] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newView.r[3] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(newView);
					newView = DirectX::XMMatrixInverse(&determinant, newView);

					views.push_back(newView);
				}
				else if (lineInfo.compare("POINT") == 0)
				{
					DirectX::XMMATRIX newPointLight;

					std::getline(inputStream, objName);
					int periodPos = objName.find_last_of('.');
					if (periodPos != -1)
						objName.erase(periodPos);

					DirectX::XMVECTOR vector;
					std::string stringVector;
					std::string extractedNumber;
					std::vector<std::string> coordinates;

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newPointLight.r[0] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newPointLight.r[1] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newPointLight.r[2] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newPointLight.r[3] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					pointLights.push_back(newPointLight);
				}
				else if (lineInfo.compare("SPOT") == 0)
				{
					DirectX::XMMATRIX newSpotLight;

					std::getline(inputStream, objName);
					int periodPos = objName.find_last_of('.');
					if (periodPos != -1)
						objName.erase(periodPos);

					DirectX::XMVECTOR vector;
					std::string stringVector;
					std::string extractedNumber;
					std::vector<std::string> coordinates;

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newSpotLight.r[0] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newSpotLight.r[1] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newSpotLight.r[2] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					std::getline(inputStream, lineInfo);
					stringVector = lineInfo;
					stringVector = stringVector.substr(13, 256);
					stringVector.erase(stringVector.length() - 1);
					stringVector.erase(stringVector.length() - 1);
					coordinates = SplitString(stringVector, ',');
					newSpotLight.r[3] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3]) };

					pointLights.push_back(newSpotLight);
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

	void CreateSkyBox(Model& skybox)
	{
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmd;
		d3d.GetCommandList((void**)&cmd);
		ID3D12Device* creator;
		d3d.GetDevice((void**)&creator);
		ID3D12CommandQueue* queue;
		d3d.GetCommandQueue((void**)&queue);
		ID3D12CommandAllocator* allocator;
		d3d.GetCommandAllocator((void**)&allocator);

		std::string pixelShader = ShaderAsString("../TexturePixelShader.hlsl");
		std::string vertexShader = ShaderAsString("../TextureVertexShader.hlsl");

		// Create Vertex Shader
		UINT compilerFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
		compilerFlags |= D3DCOMPILE_DEBUG;
#endif

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

		parser.Parse("../Models/skybox.h2b");

		skybox.meshData.world = DirectX::XMMatrixIdentity();
		skybox.meshData.world *= 95;

		//index buffer
		skybox.indexCount = parser.indexCount;
		skybox.indices = parser.indices;

		creator->CreateCommittedResource( // using UPLOAD heap for simplicity
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // DEFAULT recommend  
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(unsigned int) * skybox.indices.size()),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&(skybox.indexBuffer)));
		// Transfer triangle data to the index buffer.
		UINT8* memoryLocation;
		skybox.indexBuffer->Map(0, &CD3DX12_RANGE(0, 0),
			reinterpret_cast<void**>(&memoryLocation));
		memcpy(memoryLocation, skybox.indices.data(), sizeof(unsigned int) * skybox.indices.size());
		skybox.indexBuffer->Unmap(0, nullptr);
		// Create a index View to send to a Draw() call.
		skybox.indexView.BufferLocation = skybox.indexBuffer->GetGPUVirtualAddress();
		skybox.indexView.SizeInBytes = sizeof(unsigned int) * skybox.indices.size(); // TODO: Part 1d
		skybox.indexView.Format = DXGI_FORMAT_R32_UINT;

		//create the vertex buffer (green check)
		skybox.vertexCount = parser.vertexCount;
		skybox.vertices = parser.vertices;
		creator->CreateCommittedResource( // using UPLOAD heap for simplicity
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // DEFAULT recommend  
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(H2B::VERTEX) * skybox.vertices.size()),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&(skybox.vertexBuffer)));
		// Transfer triangle data to the vertex buffer.
		UINT8* transferMemoryLocation;
		skybox.vertexBuffer->Map(0, &CD3DX12_RANGE(0, 0),
			reinterpret_cast<void**>(&transferMemoryLocation));
		memcpy(transferMemoryLocation, skybox.vertices.data(), sizeof(H2B::VERTEX) * skybox.vertices.size());
		skybox.vertexBuffer->Unmap(0, nullptr);
		// Create a vertex View to send to a Draw() call.
		skybox.vertexView.BufferLocation = skybox.vertexBuffer->GetGPUVirtualAddress();
		skybox.vertexView.StrideInBytes = sizeof(H2B::VERTEX); // TODO: Part 1e
		skybox.vertexView.SizeInBytes = sizeof(H2B::VERTEX) * skybox.vertices.size(); // TODO: Part 1d

		skybox.materialCount = parser.materialCount;
		skybox.materials = parser.materials;

		skybox.meshCount = parser.meshCount;
		skybox.meshes = parser.meshes;

		//constant buffer
		IDXGISwapChain4* swapChain;
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		d3d.GetSwapchain4((void**)&swapChain);
		swapChain->GetDesc(&swapChainDesc);
		unsigned int bufferByteSize = (sizeof(sceneData) + skybox.meshCount * sizeof(MESH_DATA)) * swapChainDesc.BufferCount;
		creator->CreateCommittedResource( // using UPLOAD heap for simplicity
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // DEFAULT recommend  
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferByteSize),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&(skybox.constantBuffer)));
		// Transfer triangle data to the index buffer.
		UINT8* constantBufferMemoryLocation;
		skybox.constantBuffer->Map(0, &CD3DX12_RANGE(0, 0),
			reinterpret_cast<void**>(&constantBufferMemoryLocation));
		memcpy(constantBufferMemoryLocation, &sceneData, sizeof(sceneData));
		constantBufferMemoryLocation += sizeof(sceneData);
		for (int i = 0; i < skybox.meshCount; i++)
		{
			skybox.meshData.material = skybox.materials[i].attrib;
			memcpy(constantBufferMemoryLocation, &skybox.meshData, sizeof(MESH_DATA));
			constantBufferMemoryLocation += sizeof(MESH_DATA);
		}

		skybox.constantBuffer->Unmap(0, nullptr);

		//CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSceneHandle(skybox.descriptorHeap[0]->GetCPUDescriptorHandleForHeapStart(), 0, cbvDescriptorSize);

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
		descriptorHeapDesc.NumDescriptors = swapChainDesc.BufferCount;
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		creator->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&(skybox.descriptorHeap[0])));
		cbvDescriptorSize = creator->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		D3D12_CONSTANT_BUFFER_VIEW_DESC bufferDesc;
		bufferDesc.BufferLocation = skybox.constantBuffer.Get()->GetGPUVirtualAddress();
		bufferDesc.SizeInBytes = bufferByteSize;

		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = skybox.descriptorHeap[0]->GetCPUDescriptorHandleForHeapStart();

		creator->CreateConstantBufferView(&bufferDesc, descriptorHandle);

		D3D12_INPUT_ELEMENT_DESC textureFormat[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		CD3DX12_DESCRIPTOR_RANGE rootRanges[1] = { };
		rootRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

		CD3DX12_ROOT_PARAMETER textureRootParams[2] = { };
		textureRootParams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
		textureRootParams[1].InitAsDescriptorTable(1, &rootRanges[0], D3D12_SHADER_VISIBILITY_PIXEL);

		// static samplers
		CD3DX12_STATIC_SAMPLER_DESC sampler(														// TODO
			0,
			D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			0.0f, 16U,
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
			0.0f, 3.402823466e+38F,
			D3D12_SHADER_VISIBILITY_PIXEL,
			0);

		// create root signature
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(ARRAYSIZE(textureRootParams), textureRootParams, 1, &sampler,			// TODO 
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		Microsoft::WRL::ComPtr<ID3DBlob> signature;
		D3D12SerializeRootSignature(&rootSignatureDesc,
			D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errors);
		creator->CreateRootSignature(0, signature->GetBufferPointer(),
			signature->GetBufferSize(), IID_PPV_ARGS(&textureRootSignature));
		// create pipeline state
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psDesc;
		ZeroMemory(&psDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		psDesc.InputLayout = { textureFormat, ARRAYSIZE(textureFormat) };
		psDesc.pRootSignature = textureRootSignature.Get();
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
		creator->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(&texturePipeline));

		//Texture loading begin
		allocator->Reset();
		cmd->Reset(allocator, nullptr);

		bool isCubeMap = true;
		LoadTexture(creator, cmd, L"../Textures/skybox.dds", textureResource, textureUpload, &isCubeMap);

		D3D12_RESOURCE_DESC resourceDesc = textureResource->GetDesc();
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = D3D12_SHADER_RESOURCE_VIEW_DESC();
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = resourceDesc.Format;
		srvDesc.ViewDimension = (isCubeMap) ? D3D12_SRV_DIMENSION_TEXTURECUBE : D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = resourceDesc.MipLevels;
		CD3DX12_CPU_DESCRIPTOR_HANDLE descHandle(skybox.descriptorHeap[0]->GetCPUDescriptorHandleForHeapStart(), 1, cbvDescriptorSize);
		creator->CreateShaderResourceView(textureResource.Get(), &srvDesc, descHandle);

		cmd->Close();
		Microsoft::WRL::ComPtr<ID3D12CommandList> lists[] = { cmd };
		queue->ExecuteCommandLists(ARRAYSIZE(lists), lists->GetAddressOf());
		//Texture loading end

		creator->Release();
		cmd->Release();
		queue->Release();
	}

	HRESULT LoadTexture(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmd,
		const std::wstring filepath, Microsoft::WRL::ComPtr<ID3D12Resource>& resource, Microsoft::WRL::ComPtr<ID3D12Resource>& upload, bool* IsCubeMap)
	{
		HRESULT hr = E_NOTIMPL;

		std::unique_ptr<uint8_t[]> ddsData;
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		DirectX::DDS_ALPHA_MODE alphaMode;
		hr = DirectX::LoadDDSTextureFromFile(device.Get(), filepath.c_str(), resource.GetAddressOf(),
			ddsData, subresources, 0Ui64, &alphaMode, IsCubeMap);

		D3D12_RESOURCE_DESC resourceDesc = resource->GetDesc();
		CD3DX12_HEAP_PROPERTIES upload_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		UINT64 uploadSize = GetRequiredIntermediateSize(resource.Get(), 0, resourceDesc.MipLevels * resourceDesc.DepthOrArraySize);
		CD3DX12_RESOURCE_DESC upload_resource = CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

		// create a heap for uploading
		device->CreateCommittedResource(
			&upload_prop,
			D3D12_HEAP_FLAG_NONE,
			&upload_resource,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(upload.ReleaseAndGetAddressOf()));

		// update the resource using the upload heap
		UINT64 n = UpdateSubresources(cmd.Get(),
			resource.Get(), upload.Get(),
			0, 0, resourceDesc.MipLevels * resourceDesc.DepthOrArraySize,
			subresources.data());

		CD3DX12_RESOURCE_BARRIER resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmd->ResourceBarrier(1, &resource_barrier);

		cmd->DiscardResource(upload.Get(), nullptr);

		return S_OK;
	}

	void SelectLevel()
	{
		float f1Key = 0;
		input.GetState(G_KEY_F1, f1Key);

		if (f1Key != 1)
			return;

		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
			COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileOpenDialog* pFileOpen;

			// Create the FileOpenDialog object.
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
				IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

			if (SUCCEEDED(hr))
			{
				// Show the Open dialog box.
				hr = pFileOpen->Show(NULL);

				// Get the file name from the dialog box.
				if (SUCCEEDED(hr))
				{
					IShellItem* pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						LPWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

						// Display the file name to the user.
						if (SUCCEEDED(hr))
						{
							//wcstombs(levelName, pszFilePath, 500);
							std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
							std::string stringified = convert.to_bytes(pszFilePath);

							levelName = stringified;
						}
						pItem->Release();
					}
				}
				pFileOpen->Release();
			}
			CoUninitialize();
		}
		models.clear();
		ParseData();
	}
};
