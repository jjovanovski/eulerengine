#include <iostream>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <graphics/Vertex.h>
#include <graphics/AnimatedVertex.h>
#include <graphics/Animation.h>
#include <math/Mat4.h>
#include <math/Quaternion.h>

struct Mesh
{
	std::vector<Euler::Vertex> Vertices;
	std::vector<uint32_t> Indices;
};

struct AnimatedMesh
{
	std::vector<Euler::AnimatedVertex> Vertices;
	std::vector<uint32_t> Indices;
	std::vector<Mat4> BoneTransforms;
};

void ProcessScene(std::string filePath, const aiScene* scene);
void ProcessSceneWithAnimations(std::string filePath, const aiScene* scene);
void AddBoneToVertex(Euler::AnimatedVertex* vertex, int boneId, float weight);

int main(int argc, char** argv)
{
	Assimp::Importer importer;

	std::cout << "Enter file path: ";

	std::string filePath;
	std::cin >> filePath;

	std::cout << "Reading file " << filePath << std::endl;

	const aiScene* scene = importer.ReadFile(
		filePath,
		0
		| aiProcess_MakeLeftHanded
		| aiProcess_Triangulate
		| aiProcess_FlipWindingOrder
		| aiProcess_FlipUVs
		| aiProcess_JoinIdenticalVertices
	);

	if (!scene) 
	{
		std::cout << "Failed to load " << filePath << std::endl;
		return 0;
	}

	if (scene->mNumMeshes <= 0)
	{
		std::cout << "No meshes found" << std::endl;
		return 0;
	}

	/* === print file info === */

	std::cout << "File loaded" << std::endl;
	std::cout << "Meshes: " << scene->mNumMeshes << std::endl;
	std::cout << "Animations: " << scene->HasAnimations() << std::endl;

	if (scene->HasAnimations())
	{
		ProcessSceneWithAnimations(filePath, scene);
	}
	else
	{
		ProcessScene(filePath, scene);
	}

	std::cout << "Completed" << std::endl;
}

void ProcessScene(std::string filePath, const aiScene* scene)
{
	/* === convert model to our data structures === */
	std::vector<Mesh> meshes(scene->mNumMeshes);

	for (int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
	{
		aiMesh* aiMesh = scene->mMeshes[meshIndex];

		std::cout << "--- Mesh " << meshIndex << " ---" << std::endl;
		std::cout << "Vertices: " << aiMesh->mNumVertices << std::endl;
		std::cout << "Has positions: " << aiMesh->HasPositions() << std::endl;
		std::cout << "Has normals: " << aiMesh->HasNormals() << std::endl;
		std::cout << "Has UVs: " << aiMesh->HasTextureCoords(0) << std::endl;
		std::cout << "Faces (triangles): " << aiMesh->mNumFaces << std::endl;
		std::cout << "Bones: " << aiMesh->mNumBones << std::endl;
		std::cout << "Anim meshes: " << aiMesh->mNumAnimMeshes << std::endl;

		Mesh* mesh = &meshes[meshIndex];

		mesh->Vertices.resize(aiMesh->mNumVertices);
		mesh->Indices.resize(aiMesh->mNumFaces * 3);

		// write verts
		for (int i = 0; i < aiMesh->mNumVertices; i++)
		{
			auto position = aiMesh->mVertices[i];
			mesh->Vertices[i].Position = Vec3(position.x, position.y, position.z);

			if (aiMesh->mNormals != nullptr)
			{
				auto normal = aiMesh->mNormals[i];
				mesh->Vertices[i].Normal = Vec3(normal.x, normal.y, normal.z);
			}

			if (aiMesh->mTextureCoords != nullptr && aiMesh->mTextureCoords[0] != nullptr)
			{
				auto uv = aiMesh->mTextureCoords[0][i];
				mesh->Vertices[i].UV = Vec2(uv.x, uv.y);
			}
		}

		// write indices
		for (int i = 0; i < aiMesh->mNumFaces * 3; i += 3)
		{
			auto face = aiMesh->mFaces[i / 3];

			mesh->Indices[i + 0] = face.mIndices[0];
			mesh->Indices[i + 1] = face.mIndices[1];
			mesh->Indices[i + 2] = face.mIndices[2];
		}
	}

	/* === write the .bem (binary euler model) file === */


	std::string fileName = filePath.substr(0, filePath.length() - filePath.find_last_of(".") + 1) + ".bem";
	std::cout << "Writing output file " << fileName << std::endl;

	std::ofstream bfs(fileName, std::ios::out | std::ios::binary);

	uint32_t numberOfMeshes = meshes.size();
	bfs.write((const char*)(&numberOfMeshes), sizeof(numberOfMeshes));

	for (int meshIndex = 0; meshIndex < numberOfMeshes; meshIndex++)
	{
		Mesh* mesh = &meshes[meshIndex];

		uint32_t vertexCount = mesh->Vertices.size();
		uint32_t indexCount = mesh->Indices.size();

		bfs.write((const char*)(&vertexCount), sizeof(vertexCount));
		bfs.write((const char*)(&indexCount), sizeof(indexCount));

		bfs.write((const char*)mesh->Vertices.data(), mesh->Vertices.size() * sizeof(Euler::Vertex));
		bfs.write((const char*)mesh->Indices.data(), mesh->Indices.size() * sizeof(uint32_t));
	}

	bfs.close();
}

void ProcessSceneWithAnimations(std::string filePath, const aiScene* scene)
{
	/* === convert model to our data structures === */
	std::vector<AnimatedMesh> meshes(scene->mNumMeshes);

	std::map<std::string, int> boneNameToIndex;

	for (int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
	{
		aiMesh* aiMesh = scene->mMeshes[meshIndex];

		std::cout << "--- Mesh " << meshIndex << " ---" << std::endl;
		std::cout << "Vertices: " << aiMesh->mNumVertices << std::endl;
		std::cout << "Has positions: " << aiMesh->HasPositions() << std::endl;
		std::cout << "Has normals: " << aiMesh->HasNormals() << std::endl;
		std::cout << "Has UVs: " << aiMesh->HasTextureCoords(0) << std::endl;
		std::cout << "Faces (triangles): " << aiMesh->mNumFaces << std::endl;
		std::cout << "Bones: " << aiMesh->mNumBones << std::endl;
		std::cout << "Anim meshes: " << aiMesh->mNumAnimMeshes << std::endl;

		AnimatedMesh* mesh = &meshes[meshIndex];

		mesh->Vertices.resize(aiMesh->mNumVertices);
		mesh->Indices.resize(aiMesh->mNumFaces * 3);

		// write verts
		for (int i = 0; i < aiMesh->mNumVertices; i++)
		{
			auto position = aiMesh->mVertices[i];
			mesh->Vertices[i].Position = Vec3(position.x, position.y, position.z);

			if (aiMesh->mNormals != nullptr)
			{
				auto normal = aiMesh->mNormals[i];
				mesh->Vertices[i].Normal = Vec3(normal.x, normal.y, normal.z);
			}

			if (aiMesh->mTextureCoords != nullptr && aiMesh->mTextureCoords[0] != nullptr)
			{
				auto uv = aiMesh->mTextureCoords[0][i];
				mesh->Vertices[i].UV = Vec2(uv.x, uv.y);
			}

			mesh->Vertices[i].BoneIds = Vec3i(-1, -1, -1);
		}

		// write indices
		for (int i = 0; i < aiMesh->mNumFaces * 3; i += 3)
		{
			auto face = aiMesh->mFaces[i / 3];

			mesh->Indices[i + 0] = face.mIndices[0];
			mesh->Indices[i + 1] = face.mIndices[1];
			mesh->Indices[i + 2] = face.mIndices[2];
		}

		// write bones
		mesh->BoneTransforms.resize(aiMesh->mNumBones);
		for (int i = 0; i < aiMesh->mNumBones; i++)
		{
			aiBone* aiBone = aiMesh->mBones[i];
			
			std::string boneName(aiBone->mName.C_Str());
			if (boneNameToIndex.find(boneName) == boneNameToIndex.end())
			{
				boneNameToIndex[boneName] = i;
			}

			for (int j = 0; j < aiBone->mNumWeights; j++)
			{
				aiVertexWeight* weight = &aiBone->mWeights[j];
				AddBoneToVertex(&mesh->Vertices[weight->mVertexId], i, weight->mWeight);
			}
		}
	}

	std::vector<Euler::Animation*> animations;
	for (int animIndex = 0; animIndex < scene->mNumAnimations; animIndex++)
	{
		aiAnimation* animation = scene->mAnimations[animIndex];

		std::cout << "--- Animation " << animIndex << " ---" << std::endl;
		std::cout << "Name: " << animation->mName.C_Str() << std::endl;
		std::cout << "Duration: " << animation->mDuration << std::endl;
		std::cout << "Channels: " << animation->mNumChannels << std::endl;
		std::cout << "Mesh channels: " << animation->mNumMeshChannels << std::endl;


		//for (int i = 0; i < animation->mNumChannels; i++)
		//{
		//	aiNodeAnim* nodeAnim = animation->mChannels[i];
		//	std::cout << nodeAnim->mNodeName.C_Str() << " ";

		//	for (int j = 0; j < nodeAnim->mNumPositionKeys && j < 25; j++)
		//	{
		//		std::cout << nodeAnim->mPositionKeys[j].mTime << " ";
		//	}

		//	std::cout << std::endl;
		//}

		unsigned int maxKeyFrames = 1;
		for (int i = 0; i < animation->mNumChannels; i++)
		{
			aiNodeAnim* nodeAnim = animation->mChannels[i];
			maxKeyFrames = std::max(maxKeyFrames, nodeAnim->mNumPositionKeys);
			maxKeyFrames = std::max(maxKeyFrames, nodeAnim->mNumRotationKeys);
		}
		std::cout << "Key frames: " << maxKeyFrames << std::endl;

		Euler::Animation* anim = new Euler::Animation(maxKeyFrames);
		anim->Duration = animation->mDuration;
		animations.push_back(anim);

		for (int i = 0; i < maxKeyFrames; i++)
		{
			Euler::KeyFrame* keyFrame = &anim->KeyFrames[i];

			for (int j = 0; j < animation->mNumChannels; j++)
			{
				aiNodeAnim* nodeAnim = animation->mChannels[j];
				std::string boneName(nodeAnim->mNodeName.C_Str());

				if (boneNameToIndex.find(boneName) != boneNameToIndex.end())
				{
					int boneIndex = boneNameToIndex[boneName];

					auto pos = nodeAnim->mPositionKeys[i].mValue;
					keyFrame->BoneTransforms[boneIndex].Position = Vec3(pos.x, pos.y, pos.z);

					auto rot = nodeAnim->mRotationKeys[i].mValue;
					keyFrame->BoneTransforms[boneIndex].Rotation = Euler::Quaternion(rot.w, rot.x, rot.y, rot.z);
				}
			}
		}
	}

	/* === write the .beam (binary euler animated model) file === */

	std::string fileName = filePath.substr(0, filePath.length() - filePath.find_last_of(".") + 1) + ".beam";
	std::cout << "Writing output file " << fileName << std::endl;

	std::ofstream bfs(fileName, std::ios::out | std::ios::binary);

	uint32_t numberOfMeshes = meshes.size();
	bfs.write((const char*)(&numberOfMeshes), sizeof(numberOfMeshes));

	for (int meshIndex = 0; meshIndex < numberOfMeshes; meshIndex++)
	{
		AnimatedMesh* mesh = &meshes[meshIndex];

		uint32_t vertexCount = mesh->Vertices.size();
		uint32_t indexCount = mesh->Indices.size();

		bfs.write((const char*)(&vertexCount), sizeof(vertexCount));
		bfs.write((const char*)(&indexCount), sizeof(indexCount));

		bfs.write((const char*)mesh->Vertices.data(), mesh->Vertices.size() * sizeof(Euler::AnimatedVertex));
		bfs.write((const char*)mesh->Indices.data(), mesh->Indices.size() * sizeof(uint32_t));
	}

	uint32_t numberOfAnimations = animations.size();
	bfs.write((const char*)(&numberOfAnimations), sizeof(numberOfAnimations));

	/*
	* animation_duration
	* keyframe_count
	* [
	*	
	* ]
	*/

	for (Euler::Animation* animation : animations)
	{
		bfs.write((const char*)(&animation->Duration), sizeof(animation->Duration));
		bfs.write((const char*)(&animation->KeyFrameCount), sizeof(animation->KeyFrameCount));

		for (int i = 0; i < animation->KeyFrameCount; i++)
		{
			Euler::KeyFrame* keyFrame = &animation->KeyFrames[i];
			bfs.write((const char*)keyFrame, sizeof(Euler::KeyFrame));
		}
	}

	bfs.close();

	// delete animations
	for (Euler::Animation* animation : animations)
	{
		delete animation;
	}
}

struct VertexBoneData
{
	int boneId;
	float weight;

	bool operator < (const VertexBoneData& other) const
	{
		return weight < other.weight;
	}
};

void AddBoneToVertex(Euler::AnimatedVertex* vertex, int boneId, float weight)
{
	std::vector<VertexBoneData> boneData;
	boneData.push_back({ vertex->BoneIds.x, vertex->BoneWeights.x });
	boneData.push_back({ vertex->BoneIds.y, vertex->BoneWeights.y });
	boneData.push_back({ vertex->BoneIds.z, vertex->BoneWeights.z });

	bool boneAlreadyAdded = false;
	for (int i = 0; i < boneData.size(); i++)
	{
		if (boneData[i].boneId == boneId)
		{
			boneAlreadyAdded = true;
			break;
		}
	}
	if (boneAlreadyAdded)
	{
		std::cout << "Error: Bone already added" << std::endl;
		return;
	}

	boneData.push_back({ boneId, weight });
	std::sort(boneData.begin(), boneData.end());

	vertex->BoneIds.x = boneData[0].boneId;
	vertex->BoneWeights.x = boneData[0].weight;

	vertex->BoneIds.y = boneData[1].boneId;
	vertex->BoneWeights.y = boneData[1].weight;

	vertex->BoneIds.z = boneData[2].boneId;
	vertex->BoneWeights.z = boneData[2].weight;
}