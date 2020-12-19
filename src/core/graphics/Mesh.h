#pragma once

#include "../API.h"
#include "Vertex.h"

#include <vector>

namespace Euler
{
	class EULER_API Mesh
	{
	public:
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
		// TODO: Material
	};
};