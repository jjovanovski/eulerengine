#pragma once

#include "../API.h"
#include "../math/Vec3.h"
#include "../math/Vec2.h"

namespace Euler
{
	class EULER_API Vertex
	{
	public:
		Vec3 Position;
		Vec3 Normal;
		Vec2 UV;

		Vertex();
		Vertex(Vec3 position, Vec3 normal, Vec2 uv);
	};
};