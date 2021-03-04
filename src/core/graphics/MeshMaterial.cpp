#include "MeshMaterial.h"

using namespace Euler::Graphics;

MeshMaterial::MeshMaterial()
{
	Mesh = nullptr;
	ColorTexture = nullptr;
}

MeshMaterial::MeshMaterial(Euler::Mesh* mesh, Euler::Graphics::Material* material)
{
	Mesh = mesh;
	Material = material;
}

MeshMaterial::MeshMaterial(Euler::Mesh* mesh, Euler::Graphics::Texture* texture)
{
	Mesh = mesh;
	ColorTexture = texture;
}

MeshMaterial::MeshMaterial(Euler::AnimatedMesh* mesh, Euler::Graphics::Texture* texture)
{
	AnimatedMesh = mesh;
	ColorTexture = texture;
}