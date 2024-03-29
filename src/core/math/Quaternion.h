#pragma once

#include "../API.h"

#include "Vec3.h"
#include "Mat4.h"

namespace Euler {

	class EULER_API Quaternion {
	public:
		float w, x, y, z;

	public:
		Quaternion(float w = 1, float x = 0, float y = 0, float z = 0);
		~Quaternion();

		friend EULER_API Quaternion operator*(const Quaternion& left, const Quaternion& right);
		//friend EULER_API Quaternion operator*(const Quaternion& left, const Vec3& right);
		friend EULER_API Vec3 operator*(const Quaternion& left, const Vec3& right);

		float Length();
		float LengthSquared();
		void Normalize();
		Quaternion Normalized();
		Quaternion Conjugate();

		void SetEuler(float angle, const Vec3& axis);
		static Quaternion Euler(float angle, const Vec3& axis);
		Mat4 GetMatrix() const;

		static Quaternion FromMatrix(const Mat4& matrix);

		static Quaternion Slerp(Quaternion& q1, Quaternion& q2, float t);
	};

}