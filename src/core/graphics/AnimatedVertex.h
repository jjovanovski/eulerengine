#pragma once

#include "../API.h"
#include "../math/Vec3.h"
#include "../math/Vec2.h"

namespace Euler
{
	class EULER_API AnimatedVertex
	{
	public:
		Vec3 Position;
		Vec3 Normal;
		Vec3 Tangent;
		Vec3 Bitangent;
		Vec2 UV;
		Vec3i BoneIds;
		Vec3 BoneWeights;

		AnimatedVertex();
		AnimatedVertex(Vec3 position, Vec3 normal, Vec2 uv, Vec3i boneIds, Vec3 boneWeights);
	};
};