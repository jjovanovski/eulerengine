#pragma once

#include "../API.h"
#include "Mesh.h"
#include "Texture.h"

namespace Euler
{
	namespace Graphics
	{
		class EULER_API MeshMaterial
		{
		public:
			Mesh* Mesh;
			Texture* Texture;

			MeshMaterial();
			MeshMaterial(Euler::Mesh* mesh, Euler::Graphics::Texture* texture);
		};
	}
}