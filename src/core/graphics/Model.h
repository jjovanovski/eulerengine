#pragma once

#include "../API.h"
#include "MeshMaterial.h"
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

		std::vector<Graphics::MeshMaterial*> Drawables;

		Model();

		Mat4 GetModelMatrix();
	};
};