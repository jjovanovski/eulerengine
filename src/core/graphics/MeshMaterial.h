#pragma once

#include "../API.h"
#include "Mesh.h"
#include "AnimatedMesh.h"
#include "Texture.h"

namespace Euler
{
	namespace Graphics
	{
		class EULER_API MeshMaterial
		{
		public:
			Mesh* Mesh;
			AnimatedMesh* AnimatedMesh;
			Texture* NormalMap;
			Texture* ColorTexture;


			MeshMaterial();
			MeshMaterial(Euler::Mesh* mesh, Euler::Graphics::Texture* texture);
			MeshMaterial(Euler::AnimatedMesh* mesh, Euler::Graphics::Texture* texture);
		};
	}
}