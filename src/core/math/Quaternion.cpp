#include "Quaternion.h"

#include "Math.h"

using namespace Euler;

Quaternion::Quaternion(float w, float x, float y, float z) {
	this->w = w;
	this->x = x;
	this->y = y;
	this->z = z;
}

Quaternion::~Quaternion() {
}

Quaternion Euler::operator*(const Quaternion& left, const Quaternion& right) {
	float rw = left.w * right.w - left.x * right.x - left.y * right.y - left.z * right.z;
	float rx = left.x * right.w + left.w * right.x + left.y * right.z - left.z * right.y;
	float ry = left.y * right.w + left.w * right.y + left.z * right.x - left.x * right.z;
	float rz = left.z * right.w + left.w * right.z + left.x * right.y - left.y * right.x;

	return Quaternion(rw, rx, ry, rz);
}

/*Quaternion Euler::operator*(const Quaternion & left, const Vec3 & right) {
	float rw = -left.x * right.x - left.y * right.y - left.z * right.z;
	float rx = left.w * right.x + left.y * right.z - left.z * right.y;
	float ry = left.w * right.y + left.z * right.x - left.x * right.z;
	float rz = left.w * right.z + left.x * right.y - left.y * right.x;
	return Quaternion(rw, rx, ry, rz);
}*/

//Vec3 Euler::operator*(const Quaternion& left, const Vec3& right) {
//	Mat4 rot = left.GetMatrix();
//	Vec3 rotated;
//	rotated.x = rot.mat[0][0] * right.x + rot.mat[0][1] * right.y + rot.mat[0][2] * right.z;
//	rotated.y = rot.mat[1][0] * right.x + rot.mat[1][1] * right.y + rot.mat[1][2] * right.z;
//	rotated.z = rot.mat[2][0] * right.x + rot.mat[2][1] * right.y + rot.mat[2][2] * right.z;
//
//	return rotated.Normalized();
//}

float Quaternion::Length() {
	return Math::Sqrt(LengthSquared());
}

float Quaternion::LengthSquared() {
	return w * w + x * x + y * y + z * z;
}

void Quaternion::Normalize() {
	float len = Length();
	w /= len;
	x /= len;
	y /= len;
	z /= len;
}

Quaternion Quaternion::Normalized() {
	Quaternion q = *this;
	q.Normalize();
	return q;
}

Quaternion Quaternion::Conjugate() {
	Quaternion q(w, -x, -y, -z);
	return q;
}

void Quaternion::SetEuler(float angle, const Vec3& axis) {
	float sinHalfAngle = Math::Sin(angle / 2.0f);
	float cosHalfAngle = Math::Cos(angle / 2.0f);

	this->w = cosHalfAngle;
	this->x = axis.x * sinHalfAngle;
	this->y = axis.y * sinHalfAngle;
	this->z = axis.z * sinHalfAngle;
}

Quaternion Euler::Quaternion::Euler(float angle, const Vec3& axis) {
	Quaternion q;
	q.SetEuler(angle, axis);
	return q;
}

Mat4 Quaternion::GetMatrix() const {
	Mat4 qmat;

	qmat.Set(0, 0, 1.0f - 2.0f * (y * y + z * z));			qmat.Set(0, 1, 2.0f * (x * y - z * w));			qmat.Set(0, 2, 2.0f * (x * z + y * w));
	qmat.Set(1, 0, 2.0f * (x * y + z * w));				qmat.Set(1, 1, 1.0f - 2.0f * (x * x + z * z));		qmat.Set(1, 2, 2.0f * (y * z - x * w));
	qmat.Set(2, 0, 2.0f * (x * z - y * w));				qmat.Set(2, 1, 2.0f * (y * z + x * w));				qmat.Set(2, 2, 1.0f - 2.0f * (x * x + y * y));

	qmat.Set(3, 3, 1);

	return qmat;
}

Quaternion Quaternion::FromMatrix(const Mat4& matrix)
{
	float m00 = matrix.Get(0, 0);
	float m01 = matrix.Get(0, 1);
	float m02 = matrix.Get(0, 2);
	float m10 = matrix.Get(1, 0);
	float m11 = matrix.Get(1, 1);
	float m12 = matrix.Get(1, 2);
	float m20 = matrix.Get(2, 0);
	float m21 = matrix.Get(2, 1);
	float m22 = matrix.Get(2, 2);

	float trace = m00 + m11 + m22;

	if (trace > 0.0f)
	{
		float k = 0.5f / Math::Sqrt(1.0f + trace);
		return Quaternion(
			k * (m12 - m21), 
			k * (m20 - m02), 
			k * (m01 - m10), 
			0.25f / k
		);
	}
	else if ((m00 > m11) && (m00 > m22))
	{
		float k = 0.5f / Math::Sqrt(1.0f + m00 - m11 - m22);
		return Quaternion(
			0.25f / k,
			k * (m10 + m01),
			k * (m20 + m02),
			k * (m12 - m21)
		);
	}
	else if (m11 > m22)
	{
		float k = 0.5f / Math::Sqrt(1.0f + m11 - m00 - m22);
		return Quaternion(
			k * (m10 + m01), 
			0.25f / k, 
			k * (m21 + m12), 
			k * (m20 - m02)
		);
	}
	else
	{
		float k = 0.5f / Math::Sqrt(1.0f + m22 - m00 - m11);
		return Quaternion(
			k * (m20 + m02), 
			k * (m21 + m12),
			0.25f / k, 
			k * (m01 - m10)
		);
	}
}