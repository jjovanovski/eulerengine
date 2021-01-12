#pragma once

#include "../API.h"
#include "MeshMaterial.h"
#include "../math/Vec3.h"
#include "../math/Vec2.h"
#include "../math/Transform.h"

#include <vector>

namespace Euler
{
	class EULER_API Model
	{
	public:
		Transform Transform;

		std::vector<Graphics::MeshMaterial*> Drawables;

		Model();
	};
};