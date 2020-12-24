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
	Mat4 scale = Math::Matrices::Scale(Scale.x, Scale.y, Scale.z);
	Mat4 translate = Math::Matrices::Translate(Position.x, Position.y, Position.z);
	Mat4 rotx = Math::Matrices::RotateX(Rotation.x);
	Mat4 roty = Math::Matrices::RotateY(Rotation.y);
	Mat4 rotz = Math::Matrices::RotateZ(Rotation.z);

	return translate.Multiply(rotz.Multiply(roty.Multiply(rotx.Multiply(scale))));
}