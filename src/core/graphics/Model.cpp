#include "Model.h"

#include "../math/Matrices.h"

using namespace Euler;

Model::Model()
{
	Position = Vec3(0, 0, 0);
	Rotation = Vec3(0, 0, 0);
	Scale = Vec3(1, 1, 1);
}

Mat4 Model::GetModelMatrix()
{
	Mat4 mat = Math::Matrices::Translate(Position.x, Position.y, Position.z);

	return mat;
}