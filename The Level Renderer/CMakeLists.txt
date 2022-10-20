cmake_minimum_required(VERSION 3.16)

project(Final_Project)
# by default CMake selects "ALL_BUILD" as the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
	PROPERTY VS_STARTUP_PROJECT Final_Project)

# currently using unicode in some libraries on win32 but will change soon
ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

# CMake FXC shader compilation, add any shaders you want compiled here
set(VERTEX_SHADERS 
	# add vertex shader (.hlsl) files here
	VertexShader.hlsl
	TextureVertexShader.hlsl
)
set(PIXEL_SHADERS 
	# add pixel shader (.hlsl) files here
	PixelShader.hlsl
	TexturePixelShader.hlsl
)

# Add any new C/C++ source code here
add_executable (Final_Project 
	# Header & CPP files go here
	main.cpp
	renderer.h
	Model.h
	h2bParser.h
	defines.h
	${VERTEX_SHADERS}
	${PIXEL_SHADERS}
)

# This code will tell visual studio to convert above shader .hlsl to .h files
# The produced .h files can be included and contain the compiled shader byte code
set_source_files_properties( ${VERTEX_SHADERS} PROPERTIES 
	VS_SHADER_TYPE Vertex VS_SHADER_MODEL 5.1 VS_SHADER_ENTRYPOINT main 
	VS_SHADER_ENABLE_DEBUG $<$<CONFIG:Debug>:true>
)
set_source_files_properties( ${PIXEL_SHADERS} PROPERTIES 
	VS_SHADER_TYPE Pixel VS_SHADER_MODEL 5.1 VS_SHADER_ENTRYPOINT main 
	VS_SHADER_ENABLE_DEBUG $<$<CONFIG:Debug>:true>
)

# add support for dds texture loading
include_directories(${CMAKE_SOURCE_DIR}/directxtk12/include)
# Find the libraries
find_library(DDS_LIB_D NAMES DirectXTK12 PATHS ${CMAKE_SOURCE_DIR}/directxtk12/lib/x64/Debug)
find_library(DDS_LIB_R NAMES DirectXTK12 PATHS ${CMAKE_SOURCE_DIR}/directxtk12/lib/x64/Release)
# link the ktx sdk include and lib files
target_link_libraries(Final_Project debug ${DDS_LIB_D} optimized ${DDS_LIB_R})