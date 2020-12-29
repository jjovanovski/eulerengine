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

	/* === SET VIEW MATRIX === */
	Mat4 translation = Matrices::Translate(-Transform.GetPosition().x, -Transform.GetPosition().y, -Transform.GetPosition().z);
	Mat4 rotation = Transform.GetRotation().Conjugate().GetMatrix();

	viewProj.View = rotation.Multiply(translation);
	viewProj.View.Transpose();

	/* === SET PERSPECTIVE MATRIX === */
	viewProj.Projection = Matrices::Perspective(_width, _height, _fieldOfView, _nearZ, _farZ);
	viewProj.Projection.Transpose();

	return viewProj;
}