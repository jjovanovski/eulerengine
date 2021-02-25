#include "Vertex.h"

using namespace Euler;

Vertex::Vertex()
{
}

Vertex::Vertex(Vec3 position, Vec3 normal, Vec3 tangent, Vec3 bitangent, Vec2 uv)
{
	Position = position;
	Normal = normal;
	Tangent = tangent;
	Bitangent = bitangent;
	UV = uv;
}