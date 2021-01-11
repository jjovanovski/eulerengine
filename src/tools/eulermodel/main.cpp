#include <iostream>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <graphics/Vertex.h>

struct Mesh
{
	std::vector<Euler::Vertex> Vertices;
	std::vector<uint32_t> Indices;
};

int main(int argc, char** argv)
{
	Assimp::Importer importer;

	std::cout << "Enter file path: ";

	std::string filePath = "man.obj";
	//std::cin >> filePath;

	const aiScene* scene = importer.ReadFile(
		filePath,
		0
		| aiProcess_MakeLeftHanded
		| aiProcess_Triangulate
		| aiProcess_FlipWindingOrder
		| aiProcess_JoinIdenticalVertices
		| aiProcess_FlipUVs
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

	// TODO: update this program to support multiple meshes
	
	/* === load model using assimp === */
	
	Mesh mesh;

	aiMesh* x = scene->mMeshes[0];

	mesh.Vertices.resize(x->mNumVertices);
	mesh.Indices.resize(x->mNumFaces * 3);

	for (int i = 0; i < x->mNumVertices; i++)
	{
		auto position = x->mVertices[i];
		auto normal = x->mNormals[i];
		auto uv = x->mTextureCoords[0][i];

		mesh.Vertices[i].Position = Vec3(position.x, position.y, position.z);
		mesh.Vertices[i].Normal = Vec3(normal.x, normal.y, normal.z);
		mesh.Vertices[i].UV = Vec2(uv.x, uv.y);
	}

	for (int i = 0; i < x->mNumFaces * 3; i += 3)
	{
		auto face = x->mFaces[i/3];

		mesh.Indices[i + 0] = face.mIndices[0];
		mesh.Indices[i + 1] = face.mIndices[1];
		mesh.Indices[i + 2] = face.mIndices[2];
	}

	/* === write the .eulermodel file === */

	std::ofstream fs("man.eulermodel");

	uint32_t vertexCount = mesh.Vertices.size();
	uint32_t indexCount = mesh.Indices.size();
	fs << vertexCount << " ";
	fs << indexCount << " ";

	std::cout << "Writing... (vertices: " << vertexCount << ", indices: " << indexCount << ")" << std::endl;

	for (int i = 0; i < mesh.Vertices.size(); i++)
	{
		fs << mesh.Vertices[i].Position.x << " ";
		fs << mesh.Vertices[i].Position.y << " ";
		fs << mesh.Vertices[i].Position.z << " ";

		fs << mesh.Vertices[i].Normal.x << " ";
		fs << mesh.Vertices[i].Normal.y << " ";
		fs << mesh.Vertices[i].Normal.z << " ";

		fs << mesh.Vertices[i].UV.x << " ";
		fs << mesh.Vertices[i].UV.y << " ";
	}

	for (int i = 0; i < mesh.Indices.size(); i++)
	{
		fs << mesh.Indices[i] << " ";
	}

	fs.close();
}