#include "ModelResource.h"

#include <iostream>
#include <fstream>

using namespace Euler;

void ModelResource::Load(const char* filePath)
{
	//std::ifstream fs;
	//fs.open(filePath);

	//uint32_t vertexCount;
	//uint32_t indexCount;

	//fs >> vertexCount;
	//fs >> indexCount;

	//Vertices.resize(vertexCount);
	//Indices.resize(indexCount);

	//for (uint32_t i = 0; i < vertexCount; i++)
	//{
	//	fs >> Vertices[i].Position.x;
	//	fs >> Vertices[i].Position.y;
	//	fs >> Vertices[i].Position.z;

	//	fs >> Vertices[i].Normal.x;
	//	fs >> Vertices[i].Normal.y;
	//	fs >> Vertices[i].Normal.z;

	//	fs >> Vertices[i].UV.x;
	//	fs >> Vertices[i].UV.y;
	//}

	//for (uint32_t i = 0; i < indexCount; i++)
	//{
	//	fs >> Indices[i];
	//}

	std::ifstream fs;
	fs.open(filePath, std::ios::in | std::ios::binary);

	uint32_t meshCount;
	uint32_t vertexCount;
	uint32_t indexCount;
	
	fs.read((char*)(&meshCount), sizeof(uint32_t));
	fs.read((char*)(&vertexCount), sizeof(uint32_t));
	fs.read((char*)(&indexCount), sizeof(uint32_t));

	Vertices.resize(vertexCount);
	Indices.resize(indexCount);

	fs.read((char*)Vertices.data(), vertexCount * sizeof(Vertex));
	fs.read((char*)Indices.data(), indexCount * sizeof(uint32_t));

	fs.close();
}

void ModelResource::Unload()
{
	// trick to force the vectors to free-up the memory
	std::vector<Vertex>().swap(Vertices);
	std::vector<uint32_t>().swap(Indices);
}