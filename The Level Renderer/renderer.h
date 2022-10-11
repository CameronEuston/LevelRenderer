// minimalistic code to draw a single triangle, this is not part of the API.
// required for compiling shaders on the fly, consider pre-compiling instead
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include "d3dx12.h" // official helper file provided by microsoft
#include <DirectXMath.h>

float pi = 3.14159265359;

struct Vertex
{
	float x, y, z, w;
};

struct SHADER_VARS
{
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
};

// Simple Vertex Shader
const char* vertexShaderSource = R"()";
// Simple Pixel Shader
const char* pixelShaderSource = R"()";

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

	DirectX::XMMATRIX worldMatrixFloor;
	DirectX::XMMATRIX worldMatrixWall1;
	DirectX::XMMATRIX worldMatrixWall2;
	DirectX::XMMATRIX worldMatrixWall3;
	DirectX::XMMATRIX worldMatrixWall4;
	DirectX::XMMATRIX worldMatrixCeiling;

	DirectX::XMMATRIX viewMatrix;

	DirectX::XMMATRIX projectionMatrix;

	float verticalFOV;
	float nearPlane;
	float farPlane;
	float aspectRatio;

	std::chrono::steady_clock::time_point pastTime;
	std::chrono::steady_clock::time_point nowTime;

	// what we need at a minimum to draw a triangle
	D3D12_VERTEX_BUFFER_VIEW					vertexView;
	Microsoft::WRL::ComPtr<ID3D12Resource>		vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>	rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>	pipeline;
public:
	std::vector<DirectX::XMMATRIX> meshes;
	std::vector<std::string> meshNames;

	Renderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GDirectX12Surface _d3d)
	{
		win = _win;
		d3d = _d3d;
		ID3D12Device* creator;
		d3d.GetDevice((void**)&creator);

		input.Create(win);
		controller.Create();

#pragma region grids

		worldMatrixFloor = DirectX::XMMatrixIdentity();
		worldMatrixFloor = DirectX::XMMatrixMultiply(worldMatrixFloor, DirectX::XMMatrixRotationX(90 * pi / 180));
		worldMatrixFloor = DirectX::XMMatrixMultiply(worldMatrixFloor, DirectX::XMMatrixTranslation(0, -.5f, 0));

		worldMatrixWall1 = DirectX::XMMatrixIdentity();
		worldMatrixWall1 = DirectX::XMMatrixMultiply(worldMatrixWall1, DirectX::XMMatrixTranslation(0, 0, -.5f));
		//worldMatrixWall1 = DirectX::XMMatrixMultiply(worldMatrixWall1, DirectX::XMMatrixRotationX(90 * pi / 180));

		worldMatrixWall2 = DirectX::XMMatrixIdentity();
		worldMatrixWall2 = DirectX::XMMatrixMultiply(worldMatrixWall2, DirectX::XMMatrixRotationY(90 * pi / 180));
		worldMatrixWall2 = DirectX::XMMatrixMultiply(worldMatrixWall2, DirectX::XMMatrixTranslation(-.5f, 0, 0));

		worldMatrixWall3 = DirectX::XMMatrixIdentity();
		worldMatrixWall3 = DirectX::XMMatrixMultiply(worldMatrixWall3, DirectX::XMMatrixRotationY(90 * pi / 180));
		worldMatrixWall3 = DirectX::XMMatrixMultiply(worldMatrixWall3, DirectX::XMMatrixTranslation(.5f, 0, 0));

		worldMatrixWall4 = DirectX::XMMatrixIdentity();
		worldMatrixWall4 = DirectX::XMMatrixMultiply(worldMatrixWall4, DirectX::XMMatrixTranslation(0, 0, .5f));
		//worldMatrixWall4 = DirectX::XMMatrixMultiply(worldMatrixWall4, DirectX::XMMatrixRotationX(90 * pi / 180));

		worldMatrixCeiling = DirectX::XMMatrixIdentity();
		worldMatrixCeiling = DirectX::XMMatrixMultiply(worldMatrixCeiling, DirectX::XMMatrixRotationX(90 * pi / 180));
		worldMatrixCeiling = DirectX::XMMatrixMultiply(worldMatrixCeiling, DirectX::XMMatrixTranslation(0, .5f, 0));
#pragma endregion


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

		// Create Vertex Buffer
		Vertex verts[104];

		for (int i = 0; i < 52; i += 2)
		{
			verts[i] = {-0.5f, i / 50.0f - 0.5f, 0, 1};
			verts[i + 1] = {0.5f, i / 50.0f - 0.5f, 0, 1};

			verts[i + 52] = {i / 50.0f - 0.5f, -0.5f, 0, 1};
			verts[i + 53] = { i / 50.0f - 0.5f, 0.5f, 0, 1 };;
		}

		creator->CreateCommittedResource( // using UPLOAD heap for simplicity
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // DEFAULT recommend  
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(verts)),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer));
		// Transfer triangle data to the vertex buffer.
		UINT8* transferMemoryLocation;
		vertexBuffer->Map(0, &CD3DX12_RANGE(0, 0),
			reinterpret_cast<void**>(&transferMemoryLocation));
		memcpy(transferMemoryLocation, verts, sizeof(verts));
		vertexBuffer->Unmap(0, nullptr);

		// Create a vertex View to send to a Draw() call.
		vertexView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexView.StrideInBytes = sizeof(Vertex);
		vertexView.SizeInBytes = sizeof(verts);
		// Create Vertex Shader
		UINT compilerFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
		compilerFlags |= D3DCOMPILE_DEBUG;
#endif
		std::string pixelShader = ShaderAsString("PixelShader.hlsl");
		std::string vertexShader = ShaderAsString("VertexShader.hlsl");

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
			{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		CD3DX12_ROOT_PARAMETER rootParam;
		rootParam.InitAsConstants(48, 0);
		// create root signature
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(1, &rootParam, 0, nullptr, 
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		Microsoft::WRL::ComPtr<ID3DBlob> signature;
		D3D12SerializeRootSignature(&rootSignatureDesc, 
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
		psDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		psDesc.NumRenderTargets = 1;
		psDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psDesc.SampleDesc.Count = 1;
		creator->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(&pipeline));
		// free temporary handle
		creator->Release();

		pastTime = std::chrono::high_resolution_clock::now();
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

		// now we can draw
		cmd->IASetVertexBuffers(0, 1, &vertexView);

		cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		
#pragma region drawGrid
		SHADER_VARS shaderVars = { worldMatrixFloor, viewMatrix, projectionMatrix };
		cmd->SetGraphicsRoot32BitConstants(0, 48, &shaderVars, 0);
		cmd->DrawInstanced(104, 1, 0, 0);

		shaderVars = { worldMatrixWall1, viewMatrix, projectionMatrix };
		cmd->SetGraphicsRoot32BitConstants(0, 48, &shaderVars, 0);
		cmd->DrawInstanced(104, 1, 0, 0);

		shaderVars = { worldMatrixWall2, viewMatrix, projectionMatrix };
		cmd->SetGraphicsRoot32BitConstants(0, 48, &shaderVars, 0);
		cmd->DrawInstanced(104, 1, 0, 0);

		shaderVars = { worldMatrixWall3, viewMatrix, projectionMatrix };
		cmd->SetGraphicsRoot32BitConstants(0, 48, &shaderVars, 0);
		cmd->DrawInstanced(104, 1, 0, 0);

		shaderVars = { worldMatrixWall4, viewMatrix, projectionMatrix };
		cmd->SetGraphicsRoot32BitConstants(0, 48, &shaderVars, 0);
		cmd->DrawInstanced(104, 1, 0, 0);

		shaderVars = { worldMatrixCeiling, viewMatrix, projectionMatrix };
		cmd->SetGraphicsRoot32BitConstants(0, 48, &shaderVars, 0);
		cmd->DrawInstanced(104, 1, 0, 0);
#pragma endregion


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

		pastTime = nowTime;
	}
	~Renderer()
	{
		// ComPtr will auto release so nothing to do here 
	}
};
