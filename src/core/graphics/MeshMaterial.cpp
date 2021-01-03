#include "MeshMaterial.h"

using namespace Euler::Graphics;

MeshMaterial::MeshMaterial()
{
	Mesh = nullptr;
	Texture = nullptr;
}

MeshMaterial::MeshMaterial(Euler::Mesh* mesh, Euler::Graphics::Texture* texture)
{
	Mesh = mesh;
	Texture = texture;
}