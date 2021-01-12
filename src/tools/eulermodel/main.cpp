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

	std::cout << "Completed" << std::endl;
}