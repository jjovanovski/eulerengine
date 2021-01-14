#include "AnimatedModelResource.h"

#include <iostream>
#include <fstream>

using namespace Euler;

void AnimatedModelResource::Load(const char* filePath)
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
	BoneParents.resize(MAX_BONES);
	BoneOffsetMatrices.resize(MAX_BONES);

	fs.read((char*)Vertices.data(), vertexCount * sizeof(AnimatedVertex));
	fs.read((char*)Indices.data(), indexCount * sizeof(uint32_t));
	fs.read((char*)BoneParents.data(), MAX_BONES * sizeof(int));
	fs.read((char*)BoneOffsetMatrices.data(), MAX_BONES * sizeof(Mat4));

	uint32_t animationCount;
	fs.read((char*)(&animationCount), sizeof(uint32_t));

	for (int i = 0; i < animationCount; i++)
	{
		float animationDuration;
		int keyFrameCount;

		fs.read((char*)(&animationDuration), sizeof(float));
		fs.read((char*)(&keyFrameCount), sizeof(int));

		Animation* animation = new Animation(keyFrameCount);
		fs.read((char*)animation->KeyFrames, sizeof(KeyFrame) * keyFrameCount);
		Animations.push_back(animation);
	}

	fs.close();
}

void AnimatedModelResource::Unload()
{
	// trick to force the vectors to free-up the memory
	std::vector<AnimatedVertex>().swap(Vertices);
	std::vector<uint32_t>().swap(Indices);
}