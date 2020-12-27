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
	_viewProj.View = Transform.GetModelMatrix();
	_viewProj.View.Transpose();

	_viewProj.Projection = Matrices::Perspective(_width, _height, _fieldOfView, _nearZ, _farZ);
	_viewProj.Projection.Transpose();

	return _viewProj;
}