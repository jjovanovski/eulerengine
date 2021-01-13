#pragma once

#include "../API.h"
#include "../graphics/AnimatedVertex.h"
#include "../graphics/Animation.h"

#include <vector>

namespace Euler
{
	class EULER_API AnimatedModelResource
	{
	public:
		std::vector<AnimatedVertex> Vertices;
		std::vector<uint32_t> Indices;
		std::vector<Animation*> Animations;

		void Load(const char* filePath);
		void Unload();
	};
}