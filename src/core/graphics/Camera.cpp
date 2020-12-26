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
	ViewProj viewProj;

	Mat4 translate = Math::Matrices::Translate(-Position.x, -Position.y, -Position.z);
	Mat4 rotx = Math::Matrices::RotateX(-Rotation.x);
	Mat4 roty = Math::Matrices::RotateY(-Rotation.y);
	Mat4 rotz = Math::Matrices::RotateZ(-Rotation.z);
	viewProj.View = translate.Multiply(rotz.Multiply(roty.Multiply(rotx)));

	viewProj.Projection = Matrices::Perspective(_width, _height, _fieldOfView, _nearZ, _farZ);

	viewProj.View.Transpose();
	viewProj.Projection.Transpose();

	return viewProj;
}

void Camera::LookAt(Vec3 point)
{

}