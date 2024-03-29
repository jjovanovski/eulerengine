#include "Transform.h"

using namespace Euler;

Vec3 Transform::GetPosition()
{
	return _position;
}

void Transform::SetPosition(Vec3 position)
{
	_position = position;
	_dirty = true;
}

void Transform::SetPosition(float x, float y, float z)
{
	_position.x = x;
	_position.y = y;
	_position.z = z;
	_dirty = true;
}

Quaternion Transform::GetRotation()
{
	return _rotation;
}

void Transform::SetRotation(Quaternion rotation)
{
	_rotation = rotation;
	_dirty = true;
}

Vec3 Transform::GetScale()
{
	return _scale;
}

void Transform::SetScale(Vec3 scale)
{
	_scale = scale;
	_dirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	_scale.x = x;
	_scale.y = y;
	_scale.z = z;
	_dirty = true;
}

void Transform::SetScale(float s) {
	_scale.x = s;
	_scale.y = s;
	_scale.z = s;
	_dirty = true;
}

Mat4 Transform::GetModelMatrix()
{
	CheckModelMatrix();
	return _modelMatrix;
}

void Transform::CheckModelMatrix()
{
	if (!_dirty)
		return;

	Mat4 translate = Math::Matrices::Translate(_position.x, _position.y, _position.z);
	Mat4 scale = Math::Matrices::Scale(_scale.x, _scale.y, _scale.z);
	Mat4 rotate = _rotation.GetMatrix();

	_modelMatrix = translate.Multiply(scale.Multiply(rotate));
	_modelMatrix.Transpose();

	_forward = _modelMatrix.Multiply(Vec3(0, 0, 1)).Normalized();
	_right = _modelMatrix.Multiply(Vec3(1, 0, 0)).Normalized();
	_top = _modelMatrix.Multiply(Vec3(0, 1, 0)).Normalized();

	_modelMatrix.Transpose();
	_dirty = false;
}

Vec3 Transform::Forward()
{
	CheckModelMatrix();
	return _forward;
}

Vec3 Transform::Bacward()
{
	CheckModelMatrix();
	return Vec3(-_forward.x, -_forward.y, -_forward.z);
}

Vec3 Transform::Right()
{
	CheckModelMatrix();
	return _right;
}

Vec3 Transform::Left()
{
	CheckModelMatrix();
	return Vec3(-_right.x, -_right.y, -_right.z);
}

Vec3 Transform::Top()
{
	CheckModelMatrix();
	return _top;
}

Vec3 Transform::Bottom()
{
	CheckModelMatrix();
	return Vec3(-_top.x, -_top.y, -_top.z);
}

void Transform::LookAt(Vec3 point, Vec3 up)
{
	Vec3 dir = Vec3(point.x - _position.x, point.y - _position.y, point.z - _position.z).Normalized();
	dir = Vec3(-dir.x, -dir.y, -dir.z);
	Vec3 X = up.Cross(dir).Normalized();
	Vec3 Y = dir.Cross(X).Normalized();

	Mat4 rotationMatrix;

	rotationMatrix.Set(0, 0, X.x);
	rotationMatrix.Set(1, 0, X.y);
	rotationMatrix.Set(2, 0, X.z);

	rotationMatrix.Set(0, 1, Y.x);
	rotationMatrix.Set(1, 1, Y.y);
	rotationMatrix.Set(2, 1, Y.z);

	rotationMatrix.Set(0, 2, dir.x);
	rotationMatrix.Set(1, 2, dir.y);
	rotationMatrix.Set(2, 2, dir.z);

	//modelMatrix.Set(0, 3, -_position.x);
	//modelMatrix.Set(1, 3, -_position.y);
	//modelMatrix.Set(2, 3, -_position.z);

	rotationMatrix.Set(3, 3, 1);
	rotationMatrix.Transpose();

	_rotation = Quaternion::FromMatrix(rotationMatrix);
	_dirty = true;

	//Mat4 t = Math::Matrices::Translate(-_position.x, -_position.y, -_position.z);
	//t.Transpose();
	//_modelMatrix = t.Multiply(modelMatrix);
	//_modelMatrix.Transpose();

	//_forward = _modelMatrix.Multiply(Vec3(0, 0, 1));
	//_right = _modelMatrix.Multiply(Vec3(1, 0, 0));
	//_top = _modelMatrix.Multiply(Vec3(0, 1, 0));

	//// TODO: How to update the quaternion from this matrix?
}