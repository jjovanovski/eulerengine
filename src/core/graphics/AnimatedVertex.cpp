#include "AnimatedVertex.h"

using namespace Euler;

AnimatedVertex::AnimatedVertex()
{
}

AnimatedVertex::AnimatedVertex(Vec3 position, Vec3 normal, Vec2 uv, Vec3i boneIds, Vec3 boneWeights)
{
	Position = position;
	Normal = normal;
	UV = uv;
	BoneIds = boneIds;
	BoneWeights = boneWeights;
}