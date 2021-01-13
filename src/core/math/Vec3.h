#pragma once

#include "../API.h"

#include <stdint.h>
#include <math.h>

namespace Euler
{
	namespace Math
	{
		template<typename T>
		struct EULER_API Vector3Template
		{
			T x;
			T y;
			T z;

			Vector3Template() : x(0), y(0), z(0) {}

			Vector3Template(T px, T py, T pz) : x(px), y(py), z(pz) {}

			float LengthSquared() const
			{
				return x * x + y * y + z * z;
			}

			float Length() const
			{
				return sqrtf(LengthSquared());
			}

			void Normalize()
			{
				float length = Length();
				x /= length;
				y /= length;
				z /= length;
			}

			Vector3Template Normalized() const
			{
				Vector3Template normalized(x, y, z);
				normalized.Normalize();
				return normalized;
			}

			Vector3Template Cross(const Vector3Template& other) const
			{
				Vector3Template cross;
				cross.x = y * other.z - z * other.y;
				cross.y = z * other.x - x * other.z;
				cross.z = x * other.y - y * other.x;

				return cross;
			}

			float Dot(const Vector3Template& other) const
			{
				return x * other.x + y * other.y + z * other.z;
			}

			friend Vector3Template operator+(Vector3Template lhs, const Vector3Template& rhs)
			{
				return Vector3Template(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
			}

			friend Vector3Template operator*(const float& scalar, const Vector3Template& vector)
			{
				Vector3Template r(vector.x * scalar, vector.y * scalar, vector.z * scalar);
				return r;
			}

			static Vector3Template Lerp(const Vector3Template& a, const Vector3Template& b, float t)
			{
				return (1.0f - t) * a + t * b;
			}
		};
	}
}

typedef Euler::Math::Vector3Template<float_t> Vec3;
typedef Euler::Math::Vector3Template<double_t> Vec3d;
typedef Euler::Math::Vector3Template<int32_t> Vec3i;
typedef Euler::Math::Vector3Template<uint32_t> Vec3ui;
typedef Euler::Math::Vector3Template<int64_t> Vec3i64;
typedef Euler::Math::Vector3Template<uint64_t> Vec3ui64;