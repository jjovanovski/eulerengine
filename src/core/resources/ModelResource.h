#pragma once

#include "../API.h"
#include "../graphics/Vertex.h"

#include <vector>

namespace Euler
{
	class EULER_API ModelResource
	{
	public:
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;

		void Load(const char* filePath);
		void Unload();
	};
}