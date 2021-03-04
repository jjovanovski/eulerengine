#pragma once

#include "../API.h"
#include "Mesh.h"
#include "AnimatedMesh.h"
#include "Texture.h"
#include "Material.h"

namespace Euler
{
	namespace Graphics
	{
		class EULER_API MeshMaterial
		{
		public:
			Mesh* Mesh = nullptr;
			AnimatedMesh* AnimatedMesh = nullptr;
			Texture* NormalMap = nullptr;
			Texture* ColorTexture = nullptr;

			Material* Material = nullptr;

			MeshMaterial();
			MeshMaterial(Euler::Mesh* mesh, Euler::Graphics::Material* material);
			MeshMaterial(Euler::Mesh* mesh, Euler::Graphics::Texture* texture);
			MeshMaterial(Euler::AnimatedMesh* mesh, Euler::Graphics::Texture* texture);
		};
	}
}