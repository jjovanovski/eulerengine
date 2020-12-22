#pragma once

#include "../API.h"
#include "Mesh.h"
#include "../math/Vec3.h"
#include "../math/Vec2.h"

#include <vector>

namespace Euler
{
	class EULER_API Model
	{
	public:
		Vec3 Position;
		Vec3 Rotation;
		Vec3 Scale;

		std::vector<Mesh*> Meshes;

		Model();

		Mat4 GetModelMatrix();
	};
};