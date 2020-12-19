#include "Vertex.h"

using namespace Euler;

Vertex::Vertex()
{
}

Vertex::Vertex(Vec3 position, Vec3 normal, Vec2 uv)
{
	Position = position;
	Normal = normal;
	UV = uv;
}