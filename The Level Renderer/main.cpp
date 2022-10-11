// Simple basecode showing how to create a window and attatch a d3d12surface
#define GATEWARE_ENABLE_CORE // All libraries need this
#define GATEWARE_ENABLE_SYSTEM // Graphics libs require system level libraries
#define GATEWARE_ENABLE_GRAPHICS // Enables all Graphics Libraries
// Ignore some GRAPHICS libraries we aren't going to use
#define GATEWARE_DISABLE_GDIRECTX11SURFACE // we have another template for this
#define GATEWARE_DISABLE_GOPENGLSURFACE // we have another template for this
#define GATEWARE_DISABLE_GVULKANSURFACE // we have another template for this
#define GATEWARE_DISABLE_GRASTERSURFACE // we have another template for this
#define GATEWARE_ENABLE_INPUT

// TODO: Part 2a
// TODO: Part 4a
// With what we want & what we don't defined we can include the API
#include "Gateware.h"
#include "renderer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

std::vector<std::string> SplitString(std::string s, char delimiter);

// open some namespaces to compact the code a bit
using namespace GW;
using namespace CORE;
using namespace SYSTEM;
using namespace GRAPHICS;
// lets pop a window and use D3D12 to clear to a jade colored screen
int main()
{
	std::vector<DirectX::XMMATRIX> meshes;
	std::vector<std::string> meshNames;

	std::ifstream inputStream;
	inputStream.open("GameLevel.txt");

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

				DirectX::XMVECTOR vector;
				std::string stringVector;
				std::string extractedNumber;
				std::vector<std::string> coordinates;
				
				std::getline(inputStream, lineInfo);
				stringVector = lineInfo;
				stringVector = stringVector.substr(13, 256);
				stringVector.erase(stringVector.length() - 1);
				coordinates = SplitString(stringVector, ',');
				newMesh.r[0] = { std::stof(coordinates[0]), std::stof(coordinates[1]), std::stof(coordinates[2]), std::stof(coordinates[3])};

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

				meshNames.push_back(objName);
				meshes.push_back(newMesh);

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

	

	GWindow win;
	GEventResponder msgs;
	GDirectX12Surface d3d12;
	if (+win.Create(0, 0, 800, 600, GWindowStyle::WINDOWEDBORDERED))
	{
		// TODO: Part 1a
		float clr[] = { 55/255.0f, 55/255.0f, 55/255.0f, 1 }; // start with a jade color
		msgs.Create([&](const GW::GEvent& e) {
			GW::SYSTEM::GWindow::Events q;
			//if (+e.Read(q) && q == GWindow::Events::RESIZE)
			//	clr[0] += 0.01f; // move towards a orange as they resize
		});
		win.SetWindowName("Cameron Euston - Assignment 1 - DX12");
		win.Register(msgs);
		if (+d3d12.Create(win, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT))
		{
			Renderer renderer(win, d3d12); // init
			while (+win.ProcessWindowEvents())
			{
				if (+d3d12.StartFrame())
				{
					ID3D12GraphicsCommandList* cmd;
					D3D12_CPU_DESCRIPTOR_HANDLE rtv;
					D3D12_CPU_DESCRIPTOR_HANDLE dsv;
					if (+d3d12.GetCommandList((void**)&cmd) && 
						+d3d12.GetCurrentRenderTargetView((void**)&rtv) &&
						+d3d12.GetDepthStencilView((void**)&dsv))
					{
						cmd->ClearRenderTargetView(rtv, clr, 0, nullptr);
						cmd->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1, 0, 0, nullptr);
						// TODO: Part 4b
						renderer.UpdateCamera();
						renderer.Render(); // draw
						d3d12.EndFrame(false);
						cmd->Release();
					}
				}
			}// clean-up when renderer falls off stack
		}
	}
	return 0; // that's all folks
}

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
