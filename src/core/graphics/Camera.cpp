#include "Camera.h"

#include "../math/Matrices.h"

using namespace Euler;
using namespace Euler::Math;

void Camera::Init(uint32_t width, uint32_t height, float fieldOfView, float nearZ, float farZ)
{
	_width = width;
	_height = height;
	_fieldOfView = fieldOfView;
	_nearZ = nearZ;
	_farZ = farZ;
}

ViewProj Camera::GetViewProj()
{
	_viewProj.Projection = Matrices::Perspective(_width, _height, _fieldOfView, _nearZ, _farZ);
	_viewProj.Projection.Transpose();

	return _viewProj;
}

void Camera::LookAt(Vec3 point)
{
	Vec3 dir = Vec3(point.x - Position.x, point.y - Position.y, point.z - Position.z).Normalized();
	Vec3 ndir = Vec3(-dir.x, -dir.y, -dir.z).Normalized();

	Vec3 up = Vec3(0, 1, 0);
	Vec3 X = up.Cross(dir).Normalized();
	Vec3 Y = dir.Cross(X).Normalized();

	Mat4 mat = Matrices::Identity();
	
	mat.Set(0, 0, X.x);
	mat.Set(1, 0, X.y);
	mat.Set(2, 0, X.z);

	mat.Set(0, 1, Y.x);
	mat.Set(1, 1, Y.y);
	mat.Set(2, 1, Y.z);

	mat.Set(0, 2, dir.x);
	mat.Set(1, 2, dir.y);
	mat.Set(2, 2, dir.z);
	
	mat.Set(3, 3, 1);

	//mat.Transpose();

	Mat4 t = Matrices::Translate(-Position.x, -Position.y, -Position.z);
	t.Transpose();
	_viewProj.View = t.Multiply(mat);
}